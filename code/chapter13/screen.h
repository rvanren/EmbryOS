#pragma once

// For portability reasons, EmbryOS currently only supports an "old-fashioned" terminal
// screen with a certain number of rows and columns.  Each entry contains a character
// in a certain foreground and background color.  There is a current position (aka cursor).

#include "stdio.h"
#include "syscall.h"

#define SCREEN_ROWS 24
#define SCREEN_COLS 80

// Go to the given position.  (0, 0) is top left.
void screen_move(int row, int col);

// Put a character at the current position and move up the position
void screen_put(cell_t cell);

// Clear the given rectangle with the given color.
void screen_clear(int x, int y, int w, int h, int color);
