#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_all(char *foreground, char *background);
void draw_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void inits(char *foreground, char *background);
