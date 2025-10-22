#include "syslib.h"
#include "screen.h"

static void screen_put(int row, int col, struct cell cell) {
    user_put(row, col, cell.c, cell.fg, cell.bg);
}

void screen_init(struct screen *screen) {
    screen->cur_col = 0; screen->cur_fg = 0; screen->cur_bg = 7;
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            screen->cells[row][col] = (struct cell) { ' ', 0, 7 };
}

void screen_sync(struct screen *screen) {
    for (int row = 0; row < NROWS; row++)
        for (int col = 0; col < NCOLS; col++)
            screen_put(row, col, screen->cells[row][col]);
}

void screen_scroll(struct screen *screen) {
    for (int row = 0; row < NROWS - 1; row++)
        for (int col = 0; col < NCOLS; col++)
            screen->cells[row][col] = screen->cells[row + 1][col];
    for (int col = 0; col < NCOLS; col++)
        screen->cells[NROWS - 1][col] = (struct cell) { ' ', 0, 7 };
    screen_sync(screen);
}

void screen_putchar(struct screen *screen, char c) {
    if (c == '\b' || c == '\177') {
        if (screen->cur_col > 0) {
            screen->cur_col--;
            user_put(NROWS - 1, screen->cur_col,
                ' ', screen->cur_fg, screen->cur_bg);
            screen->cells[NROWS - 1][screen->cur_col] =
                (struct cell) { ' ', screen->cur_fg, screen->cur_bg };
            if (screen->cur_col > 0)
                screen_put(NROWS - 1, screen->cur_col - 1,
                    screen->cells[NROWS - 1][screen->cur_col - 1]);
        }
        return;
    }
    if (c == '\n' || screen->cur_col == NCOLS) {
        screen_scroll(screen);
        screen->cur_col = 0;
    }
    if (c != '\n') {
        user_put(NROWS - 1, screen->cur_col, c, screen->cur_fg, screen->cur_bg);
        screen->cells[NROWS - 1][screen->cur_col] =
            (struct cell) { c, screen->cur_fg, screen->cur_bg };
        screen->cur_col++;
    }
}
