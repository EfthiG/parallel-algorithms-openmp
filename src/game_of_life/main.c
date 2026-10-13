#include "game_of_life.h"

void check_args(int argc, char* argv[]){
    if(argc != 6)
        custom_error("Usage: ./main <generations> <rows> <cols> <parallel/serial (0 or 1)> <threads>");

    if(atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0)
        custom_error("Invalid input: generations, rows, and cols must be positive integers");

    if(atoi(argv[4]) != 0 && atoi(argv[4]) != 1)
        custom_error("Invalid input: parallel must be 0 or 1");

    if(atoi(argv[5]) <= 0 && atoi(argv[4]) == 1)
        custom_error("Invalid input: threads must be a positive integer");
}


int main(int argc, char* argv[]){
    check_args(argc, argv);

    // Parse command line arguments
    int generations = atoi(argv[1]);
    int rows = atoi(argv[2]);
    int cols = atoi(argv[3]);
    bool parallel = atoi(argv[4]);
    int threads = atoi(argv[5]);

    // Create a file with random coordinates for the live cells
    char* filename = create_coords_filename(rows, cols);
    generate_random_coords(rows, cols, filename);

    // Create a grid and fill it with the live cells
    bool** current_grid = create_grid(rows, cols);
    fill_grid(current_grid, rows, cols, filename);

    // Create a second grid for the next generation
    bool** next_grid = create_grid(rows, cols);

    // Execute the game of life and measure the time
    double time = execute_game_of_life(current_grid, next_grid, rows, cols, generations, parallel, threads);

    // Print the results to a file
    print_results(rows, cols, generations, time, parallel, threads);

    // Cleanup resources
    free_memories(current_grid, next_grid, rows, filename);

    return 0;
}