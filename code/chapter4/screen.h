#ifndef SCREEN_H
#define SCREEN_H

#include "stdio.h"

#define SCREEN_ROWS 24
#define SCREEN_COLS 80

void screen_move(int row, int col);
void screen_put(char ch, int fg, int bg);
void screen_clear(int x, int y, int w, int h, int color);

#endif // SCREEN_H
