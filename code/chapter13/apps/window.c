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
            user_put(row, col, window->cells[row][col]);
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
        if (window->cur_col > 0) {
            window->cur_col--;
            user_put(NROWS - 1, window->cur_col,
                    CELL(' ', window->cur_fg, window->cur_bg));
            window->cells[NROWS - 1][window->cur_col] =
                    CELL(' ', window->cur_fg, window->cur_bg);
            if (window->cur_col > 0)
                user_put(NROWS - 1, window->cur_col - 1,
                    window->cells[NROWS - 1][window->cur_col - 1]);
        }
        return;
    }
    if (c == '\n' || window->cur_col == NCOLS) {
        window_scroll(window);
        window->cur_col = 0;
    }
    if (c != '\n') {
        user_put(NROWS - 1, window->cur_col,
                    CELL(c, window->cur_fg, window->cur_bg));
        window->cells[NROWS - 1][window->cur_col] =
                    CELL(c, window->cur_fg, window->cur_bg);
        window->cur_col++;
    }
}

void window_readline(struct window *window, char *line, int size) {
    int start_col = window->cur_col;

    int n = 0;
    for (;;) {
        char c = user_get(NROWS - 1, window->cur_col,
                    CELL(' ',  ANSI_WHITE,  ANSI_CYAN),
                    CELL('.',  ANSI_BLACK,  ANSI_MAGENTA));
        if (c == '\r') c = '\n';
        if (c == '\n') {
            line[(n < size - 1) ? n : (size - 1)] = 0;
            return;
        }
        window_putchar(window, (32 <= c && c < 127) ? c : ' ');
        if (c == '\b' || c == '\177') {
            if (n > 0) {
                window->cur_col--;
                n--;
            }
        }
        else {
            if (n < size - 1) line[n] = c;
            n++;
        }
    }
}
