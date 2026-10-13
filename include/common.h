#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <omp.h>



void custom_error(char* program_name);
void perror_exit(char* message);
void check_num(int num, char* message);
void check_ptr(void* ptr, char* message);

char* concat(char* s1, char* s2);
FILE* open_file(char* filename, char* mode);


#endif