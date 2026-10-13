#include "common.h"

void custom_error(char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void perror_exit(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void check_num(int num, char* message){
    if(num <= 0)
        custom_error(message);
}

void check_ptr(void* ptr, char* message){
    if(ptr == NULL)
        perror_exit(message);
}

char* concat(char* s1, char* s2) {
    char* result = (char*)malloc(strlen(s1) + strlen(s2) + 1);
    check_ptr(result, "malloc");

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

FILE* open_file(char* filename, char* mode) {
    FILE* file = fopen(filename, mode);
    check_ptr(file, "fopen");
    return file;
}