#include "gauss.h"

void by_line_serial(int** A, int* b, int* x, int size){
    int row, col;
    for (row = size - 1; row >= 0; row--){
        x[row] = b[row];
        for (col = row + 1; col < size; col++)
            x[row] -= A[row][col] * x[col];

        x[row] /= A[row][row];
    }
}

void by_column_serial(int** A, int* b, int* x, int size){
    for (int col = 0; col < size; col++)
        x[col] = b[col];

    for (int col = size - 1; col >= 0; col--){
        x[col] /= A[col][col];
        for (int row = 0 ; row < col; row++)
            x[row] -= A[row][col] * x[col];
    }
}

// Function to determine the schedule type based on the schedule name
omp_sched_t get_schedule_type(const char* schedule_name) {
    if (strcmp(schedule_name, "dynamic") == 0)
        return omp_sched_dynamic;
    else if (strcmp(schedule_name, "guided") == 0)
        return omp_sched_guided;
    // Default to static if the schedule name is not recognized
    else
        return omp_sched_static;
}

void by_line_parallel(int** A, int* b, int* x, int size, int threads, const char* schedule_name) {
    int row, col;
    omp_sched_t schedule_type = get_schedule_type(schedule_name);
    omp_set_schedule(schedule_type, 0); // Set OpenMP schedule

    for (row = size - 1; row >= 0; row--) {
        x[row] = b[row];
        #pragma omp parallel for num_threads(threads) schedule(runtime)
        for (col = row + 1; col < size; col++)
            #pragma omp atomic
            x[row] -= A[row][col] * x[col];

        x[row] /= A[row][row];
    }
}

void by_column_parallel(int** A, int* b, int* x, int size, int threads, const char* schedule_name) {
    omp_sched_t schedule_type = get_schedule_type(schedule_name);
    omp_set_schedule(schedule_type, 0); // Set OpenMP schedule

    #pragma omp parallel for num_threads(threads) schedule(runtime)
    for (int col = 0; col < size; col++)
        x[col] = b[col];

    for (int col = size - 1; col >= 0; col--) {
        x[col] /= A[col][col];
        #pragma omp parallel for num_threads(threads) schedule(runtime)
        for (int row = 0; row < col; row++)
            x[row] -= A[row][col] * x[col];
    }
}

// Initialize matrix A, vector b and vector x
void init_array(int*** A, int** b, int** x, int size) {
    *A = malloc(size * sizeof(int*));
    check_ptr(*A, "Matrix A cannot be allocated");
    *b = malloc(size * sizeof(int));
    check_ptr(*b, "Vector b cannot be allocated");
    *x = malloc(size * sizeof(int));
    check_ptr(*x, "Vector x cannot be allocated");

    for (int i = 0; i < size; i++){
        (*A)[i] = malloc(size * sizeof(int));
        check_ptr((*A)[i], "Matrix A row cannot be allocated");
    }

    // Seed the random number generator
    srand(time(NULL));

    // Fill the matrix and the vector with random numbers
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            (*A)[i][j] = rand() % 10;
            if (i == j && (*A)[i][j] == 0)
                (*A)[i][j] = 1; // Ensure diagonal elements are non-zero
        }
        (*b)[i] = rand() % 10;
    }

    // Fill the lower part of the matrix with 0
    for (int i = 0; i < size; i++)
        for (int j = 0; j < i; j++)
            (*A)[i][j] = 0;
}


// Print the results to a file
void print_result(int* x, int size, double time, const char* method, const char* order, int threads, const char* schedule) {
    FILE* file = open_file("results/gauss.txt", "a");

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
        fprintf(file, "%-10s %-15s %-15s %-10s %-10s %-15s\n\n", "Size", "Method", "Order", "Threads", "Time", "Schedule");

    fprintf(file, "%-10d %-15s %-15s %-10d %-10f %-15s\n", size, method, order, threads, time, schedule);
    fclose(file);
}


// Execute backward substitution and measure time
double execute_substitution(int** A, int* b, int* x, int size, const char* order, const char* method, int threads, const char* schedule) {
    double start = omp_get_wtime();
    if (strcmp(method, "serial") == 0) {
        if (strcmp(order, "by_line") == 0)
            by_line_serial(A, b, x, size);
        else
            by_column_serial(A, b, x, size);
    } else {
        if (strcmp(order, "by_line") == 0)
            by_line_parallel(A, b, x, size, threads, schedule);
        else
            by_column_parallel(A, b, x, size, threads, schedule);
    }
    double end = omp_get_wtime();
    return end - start;
}


void free_memories(int** A, int* b, int* x, int size) {
    for (int i = 0; i < size; i++)
        free(A[i]);
    free(A);
    free(b);
    free(x);
}