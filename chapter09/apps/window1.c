#include "syslib.h"
#include "window.h"

void window_init(struct window *window) {
    window->cur_col = 0; window->cur_fg = 0; window->cur_bg = 7;
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            window->cells[row][col] = CELL(' ', ANSI_BLACK, ANSI_WHITE);
}

void window_sync(struct window *window) {
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            user_put(col, row, window->cells[row][col]);
}

void window_scroll(struct window *window) {
    for (int row = 0; row < NROWS - 1; row++)
        for (int col = 0; col < NCOLS; col++)
            window->cells[row][col] = window->cells[row + 1][col];
    for (int col = 0; col < NCOLS; col++)
        window->cells[NROWS - 1][col] = CELL(' ', ANSI_BLACK, ANSI_WHITE);
    window_sync(window);
}

void window_putchar(struct window *window, char c) {
    if (c == '\b') {
        if (window->cur_col > 0) window->cur_col--;
        return;
    }
    if (c == '\n') {
        window_scroll(window);
        window->cur_col = 0;
    }
    else {
        user_put(window->cur_col, NROWS - 1,
                    CELL(c, window->cur_fg, window->cur_bg));
        window->cells[NROWS - 1][window->cur_col] =
                    CELL(c, window->cur_fg, window->cur_bg);
        if (++window->cur_col == NCOLS) {
            window_scroll(window);
            window->cur_col = 0;
        }
    }
}
