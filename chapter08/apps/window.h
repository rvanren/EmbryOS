#pragma once

#include "syscall.h"

#define NROWS 11
#define NCOLS 39

// A window is a collection of cells, each containing a character with
// a particular foreground and background color.  The window has a
// current column (the current row is always the last one).  The window
// also supports a current foreground and background color.
struct window {
    cell_t cells[NROWS][NCOLS];
    int cur_col /* column */, cur_fg, cur_bg;
};

// Initialize a window module
void window_init(struct window *window);

// Update the screen with the current contents of the window
void window_sync(struct window *window);

// Scroll the window by one row
void window_scroll(struct window *window);

// Put a character at the current position and move it up
void window_putchar(struct window *window, char c);

// Read a line and process special characters.  Return 0 on EOF.
int window_readline(struct window *window, char *line, int size);
