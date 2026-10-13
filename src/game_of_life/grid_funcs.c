#include "game_of_life.h"

// Print the grid to the console (debugging)
void print_grid(bool** grid, int rows, int cols){
    printf("--------------------\n");
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++)
            grid[i][j] ? printf("X") : printf(" ");
        printf("\n");
    }
    printf("--------------------\n");
}

// Print the grid to a file (debugging)
void print_grid_to_file(bool** grid, int rows, int cols, char* filename){
    char* file_in_folder = concat("results/", filename);
    FILE* file = open_file(file_in_folder, "w");

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++)
            grid[i][j] ? fprintf(file, "X") : fprintf(file, " ");
        fprintf(file, "\n");
    }

    fclose(file);
    free(file_in_folder);
}

// Create a grid with all cells dead
bool** create_grid(int rows, int cols){
    bool** grid = (bool**)malloc(rows * sizeof(bool*));
    check_ptr(grid, "malloc");

    for(int i = 0; i < rows; i++){
        grid[i] = (bool*)malloc(cols * sizeof(bool));
        check_ptr(grid[i], "malloc");

        for(int j = 0; j < cols; j++)
            grid[i][j] = DEAD;
    }
    return grid;
}

// Fill the grid with the live cells from the file
void fill_grid(bool** grid, int rows, int cols, char* filename){
    char* file_in_folder = concat("input_files/", filename);
    FILE* file = open_file(file_in_folder, "r");

    while(!feof(file)){
        int row, col;
        fscanf(file, "%d %d\n", &row, &col);
        if (row >= rows || row < 0 || col >= cols || col < 0)
            custom_error("Invalid coordinates in file\n");

        grid[row][col] = ALIVE;
    }

    fclose(file);
    free(file_in_folder);
}


// Concatenate the rows and cols to the filename to make it unique
char* create_coords_filename(int rows, int cols){
    int len = snprintf(NULL, 0, "coords_%d_%d.txt", rows, cols);
    char* filename = (char*)malloc(len + 1);
    check_ptr(filename, "malloc");

    sprintf(filename, "coords_%d_%d.txt", rows, cols);
    return filename;
}

// Generate random coordinates for the live cells and write them to a file
void generate_random_coords(int rows, int cols, char* filename){
    char* file_in_folder = concat("input_files/", filename);
    // Check if file already exists
    if(access(file_in_folder, F_OK) != -1){
        free(file_in_folder);
        return;
    }
    FILE* file = open_file(file_in_folder, "w");

    // Generate random coordinates for the live cells
    int len_of_live_cells = (rows * cols) / 4;
    srand(time(NULL));
    int rand_row;
    int rand_col;

    for (int i=0; i < len_of_live_cells; i++){
        rand_row = rand() % rows;
        rand_col = rand() % cols;
        fprintf(file, "%d %d\n", rand_row, rand_col);
    }

    fclose(file);
    free(file_in_folder);
}

// Count the number of alive neighbors
int count_alive_neighbours(bool** grid, int rows, int cols, int row, int col) {
    // Arracol of coordinates for each of the 8 neighbors (row, col)
    const int neighbors[8][2] = {
        {-1, -1},  // Top-left
        {-1,  0},  // Top
        {-1,  1},  // Top-right
        { 0, -1},  // Left
        { 0,  1},  // Right
        { 1, -1},  // Bottom-left
        { 1,  0},  // Bottom
        { 1,  1}   // Bottom-right
    };

    int count = 0;

    for (int i = 0; i < 8; i++) {
        int nrow = row + neighbors[i][0];  // row-coordinate of neighbor
        if(nrow < 0 || nrow >= rows)
            continue;

        int ncol = col + neighbors[i][1];  // col-coordinate of neighbor
        if(ncol < 0 || ncol >= cols)
            continue;

        count += grid[nrow][ncol];
    }

    return count;
}


// Update the grid without creating a new grid
void update_grid_serial(bool** current_grid, bool** next_grid, int rows, int cols) {
    // Serial implementation
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int alive_neighbours = count_alive_neighbours(current_grid, rows, cols, i, j);

            if(current_grid[i][j] == ALIVE)
                next_grid[i][j] = (alive_neighbours == 2 || alive_neighbours == 3) ? ALIVE : DEAD;
            else
                next_grid[i][j] = (alive_neighbours == 3) ? ALIVE : DEAD;
        }
    }
}

// // Update the grid without creating a new grid
void update_grid_parallel(bool** current_grid, bool** next_grid, int rows, int cols, int threads) {
    // Parallel implementation
    #pragma omp parallel for num_threads(threads) schedule(static) collapse(2)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int alive_neighbours = count_alive_neighbours(current_grid, rows, cols, i, j);

            if(current_grid[i][j] == ALIVE)
                next_grid[i][j] = (alive_neighbours == 2 || alive_neighbours == 3) ? ALIVE : DEAD;
            else
                next_grid[i][j] = (alive_neighbours == 3) ? ALIVE : DEAD;
        }
    }
}

// Execute the game of life for the given number of generations
double execute_game_of_life(bool** current_grid, bool** next_grid, int rows, int cols, int generations, bool parallel, int threads){
    double start = omp_get_wtime();

    for (int generation = 0; generation < generations; generation++) {
        if(parallel)
            update_grid_parallel(current_grid, next_grid, rows, cols, threads);
        else
            update_grid_serial(current_grid, next_grid, rows, cols);

        // print_grid(current_grid, rows, cols);

        // Swap the grids
        bool** temp = current_grid;
        current_grid = next_grid;
        next_grid = temp;
    }

    return omp_get_wtime() - start;
}


void print_results(int rows, int cols, int generations, double time, bool parallel, int threads){
    int len = strlen("results/game_of_life.txt") + 1;
    char* filename = malloc(len);
    check_ptr(filename, "malloc");
    strcpy(filename, "results/game_of_life.txt");

    FILE* file = open_file(filename, "a");

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
        fprintf(file, "%-10s %-10s %-15s %-10s %-10s %-10s\n\n", "Rows", "Cols", "Generations", "Parallel", "Threads", "Time");
    
    fprintf(file, "%-10d %-10d %-15d %-10d %-10d %-10f\n", rows, cols, generations, parallel ? 1 : 0, threads, time);

    fclose(file);
    free(filename);
}


// Free the memory allocated for the grid
void free_grid(bool** grid, int rows){
    for(int i = 0; i < rows; i++)
        free(grid[i]);
    free(grid);
}

void free_memories(bool** current_grid, bool** next_grid, int rows, char* filename){
    free_grid(current_grid, rows);
    free_grid(next_grid, rows);
    free(filename);
}