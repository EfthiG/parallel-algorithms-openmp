#ifndef game_of_life_H
#define game_of_life_H
#include "common.h"

#define DEAD false
#define ALIVE true

bool** create_grid(int rows, int cols);
void fill_grid(bool** grid, int rows, int cols, char* filename);
char* create_coords_filename(int rows, int cols);
void generate_random_coords(int rows, int cols, char* filename);

double execute_game_of_life(bool** current_grid, bool** next_grid, int rows, int cols, int generations, bool parallel, int threads);
void print_results(int rows, int cols, int generations, double time, bool parallel, int threads);
void free_memories(bool** current_grid, bool** next_grid, int rows, char* filename);

#endif