#include "gauss.h"

void check_args(int argc, char* argv[]) {
    if (argc != 6)
        custom_error("Usage: ./gauss <size> <serial/parallel> <by_line/by_column> <threads> <schedule>");

    if (atoi(argv[1]) <= 0)
        custom_error("Size must be greater than 0");

    if (strcmp(argv[2], "serial") != 0 && strcmp(argv[2], "parallel") != 0)
        custom_error("Second argument must be 'serial' or 'parallel'");

    if (strcmp(argv[3], "by_line") != 0 && strcmp(argv[3], "by_column") != 0)
        custom_error("Third argument must be 'by_line' or 'by_column'");

    if (strcmp(argv[2], "parallel") == 0 && atoi(argv[4]) <= 0)
        custom_error("Number of threads must be greater than 0");
}

int main(int argc, char* argv[]) {
    check_args(argc, argv);

    int** A;
    int* b, *x;

    int size = atoi(argv[1]);
    int num_threads = atoi(argv[4]);

    // Initialize matrix A, vector b and vector x
    init_array(&A, &b, &x, size);

    // Execute backward substitution and measure time
    double time = execute_substitution(A, b, x, size, argv[3], argv[2], num_threads, argv[5]);

    // Print results to a file
    print_result(x, size, time, argv[2], argv[3], num_threads, argv[5]);

    // Free memory allocated
    free_memories(A, b, x, size);
}