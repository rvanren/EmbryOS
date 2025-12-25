#pragma once

// For portability reasons, EmbryOS currently only supports an "old-fashioned" terminal
// screen with a certain number of rows and columns.  Each entry contains a character
// in a certain foreground and background color.  There is a current position (aka cursor).

#include "syscall.h"

#define SCREEN_ROWS 24
#define SCREEN_COLS 80

// Put a character at the given position
void screen_put(int row, int col, cell_t cell);

// Clear the given rectangle with the given cell.
void screen_fill(int x, int y, int w, int h, cell_t cell);
