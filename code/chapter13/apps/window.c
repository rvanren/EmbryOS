#include "syslib.h"
#include "window.h"

static void window_put(int row, int col, struct cell cell) {
    user_put(row, col, cell.c, cell.fg, cell.bg);
}

void window_init(struct window *window) {
    window->cur_col = 0; window->cur_fg = 0; window->cur_bg = 7;
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            window->cells[row][col] = (struct cell) { ' ', 0, 7 };
}

void window_sync(struct window *window) {
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            window_put(row, col, window->cells[row][col]);
}

void window_scroll(struct window *window) {
    for (int row = 0; row < NROWS - 1; row++)
        for (int col = 0; col < NCOLS; col++)
            window->cells[row][col] = window->cells[row + 1][col];
    for (int col = 0; col < NCOLS; col++)
        window->cells[NROWS - 1][col] = (struct cell) { ' ', 0, 7 };
    window_sync(window);
}

void window_putchar(struct window *window, char c) {
    if (c == '\b' || c == '\177') {
        if (window->cur_col > 0) {
            window->cur_col--;
            user_put(NROWS - 1, window->cur_col,
                ' ', window->cur_fg, window->cur_bg);
            window->cells[NROWS - 1][window->cur_col] =
                (struct cell) { ' ', window->cur_fg, window->cur_bg };
            if (window->cur_col > 0)
                window_put(NROWS - 1, window->cur_col - 1,
                    window->cells[NROWS - 1][window->cur_col - 1]);
        }
        return;
    }
    if (c == '\n' || window->cur_col == NCOLS) {
        window_scroll(window);
        window->cur_col = 0;
    }
    if (c != '\n') {
        user_put(NROWS - 1, window->cur_col, c, window->cur_fg, window->cur_bg);
        window->cells[NROWS - 1][window->cur_col] =
            (struct cell) { c, window->cur_fg, window->cur_bg };
        window->cur_col++;
    }
}
