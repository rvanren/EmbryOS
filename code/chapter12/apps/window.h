#pragma once

#define NROWS 11
#define NCOLS 39

struct cell { char c, fg, bg; };
struct window {
    struct cell cells[NROWS][NCOLS];
    int cur_col, cur_fg, cur_bg;
};

void window_init(struct window *window);
void window_sync(struct window *window);
void window_scroll(struct window *window);
void window_putchar(struct window *window, char c);
