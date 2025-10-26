#include "syslib.h"
#include "window.h"

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
            window_putchar(window, '\n');
            return;
        }
        if (c == '\b' || c == '\177') {
            if (n > 0) {
                user_put(NROWS - 1, window->cur_col,    // erase cursor
                        CELL(' ', window->cur_fg, window->cur_bg));
                window->cur_col--;
                window->cells[NROWS - 1][window->cur_col] =
                        CELL(' ', window->cur_fg, window->cur_bg));
                user_put(NROWS - 1, window->cur_col,    // erase last char
                        CELL(' ', window->cur_fg, window->cur_bg));
                n--;
            }
        }
        else {
            window_putchar(window, (32 <= c && c < 127) ? c : '?');
            if (n < size - 1) line[n] = c;
            n++;
        }
    }
}
