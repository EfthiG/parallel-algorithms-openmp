#ifndef GAUSS_H
#define GAUSS_H
#include "common.h"

void init_array(int*** A, int** b, int** x, int size);
double execute_substitution(int** A, int* b, int* x, int size, const char* order, const char* method, int threads, const char* schedule);

void print_result(int* x, int size, double time, const char* method, const char* order, int threads, const char* schedule);
void free_memories(int** A, int* b, int* x, int size);

#endif