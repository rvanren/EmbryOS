#include "syslib.h"
#include "window.h"

int window_readline(struct window *window, char *line, int size) {
    int n = 0;
    for (;;) {
        user_put(window->cur_col, NROWS - 1, CELL(' ',  ANSI_WHITE,  ANSI_CYAN));
        int c;
        for (;;) {
            c = user_get(1);
            if (c == USER_GET_GOT_FOCUS)
                user_put(window->cur_col, NROWS - 1, CELL(' ',  ANSI_WHITE,  ANSI_CYAN));
            else if (c == USER_GET_LOST_FOCUS)
                user_put(window->cur_col, NROWS - 1, CELL(' ',  ANSI_WHITE,  ANSI_MAGENTA));
            else break;
        }
        if (c == 4) {   // <ctrl>D is EOF
            line[(n < size - 1) ? n : (size - 1)] = 0;
            return n > 0;
        }
        if (c == '\b' || c == '\177') {
            if (n > 0) {
                user_put(window->cur_col, NROWS - 1,   // erase cursor
                        CELL(' ', window->cur_fg, window->cur_bg));
                window->cur_col--;
                window->cells[NROWS - 1][window->cur_col] =
                        CELL(' ', window->cur_fg, window->cur_bg);
                user_put(window->cur_col, NROWS - 1,   // erase last char
                        CELL(' ', window->cur_fg, window->cur_bg));
                n--;
            }
        }
        else {
            if (c == '\r') c = '\n';
            if (n < size - 1) line[n] = c;
            n++;
            if (c == '\n') {
                line[(n < size - 1) ? n : (size - 1)] = 0;
                window_putchar(window, '\n');
                return 1;
            }
            else window_putchar(window, (32 <= c && c < 127) ? c : '?');
        }
    }
}
