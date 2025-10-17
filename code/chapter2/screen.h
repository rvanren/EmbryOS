#include "stdio.h"

#define SCREEN_ROWS 24
#define SCREEN_COLS 80

void screen_move(int row, int col);
void screen_put(char ch, int color);
void screen_clear(int row, int col, int w, int h);
