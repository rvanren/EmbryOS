#include "syslib.h"

#define NROWS 12
#define NCOLS 40

struct cell { char c, fg, bg; };
struct cell screen[NROWS][NCOLS];
int cur_col = 0, cur_fg = 0, cur_bg = 7;

void screen_init() {
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen[row][col] = (struct cell) { ' ', 0, 7 };
        }
    }
}

void screen_put(int row, int col, struct cell cell) {
    user_put(row, col, cell.c, cell.fg, cell.bg);
}

void screen_sync() {
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen_put(row, col, screen[row][col]);
        }
    }
}

void scroll() {
    for (int row = 0; row < NROWS - 1; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen[row][col] = screen[row + 1][col];
        }
    }
    for (int col = 0; col < NCOLS; col++) {
        screen[NROWS - 1][col] = (struct cell) { ' ', 0, 7 };
    }
    screen_sync();
}

void putchar(char c) {
    if (c == '\b' || c == '\177') {
        if (cur_col > 0) {
            cur_col--;
            user_put(NROWS - 1, cur_col, ' ', cur_fg, cur_bg);
            screen[NROWS - 1][cur_col] = (struct cell) { ' ', cur_fg, cur_bg };
            if (cur_col > 0)
                screen_put(NROWS - 1, cur_col - 1, screen[NROWS - 1][cur_col - 1]);
        }
        return;
    }
    if (c == '\n' || cur_col == NCOLS) {
        scroll();
        cur_col = 0;
    }
    if (c != '\n') {
        user_put(NROWS - 1, cur_col, c, cur_fg, cur_bg);
        screen[NROWS - 1][cur_col] = (struct cell) { c, cur_fg, cur_bg };
        cur_col++;
    }
}

void exec(char *line) {
    while (*line != 0) {
        if (*line == '1') user_spawn(1, 40,  0, 40, 12);
        if (*line == '2') user_spawn(1, 40, 12, 40, 12);
        if (*line == '!') user_spawn(2,  0, 12, 40, 12);
        if (*line == '.') user_exit();
        line++;
    }
}

void main(void) {
    char line[128];
    unsigned int n;

    screen_init();
    screen_sync();
    putchar('$'); putchar(' ');
    for (;;) {
        char c = user_get();
        if (c == '\r') {
            putchar('\n');
            line[n] = 0;
            exec(line);
            n = 0;
            putchar('$');
            putchar(' ');
        }
        else {
            putchar(c);
            if (n < sizeof(line) - 1) line[n++] = c;
        }
    }
}
