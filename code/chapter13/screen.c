#include "stdio.h"
#include "screen.h"
#include "uart.h"

static int cur_row = 0, cur_col = 0;

static void term_move(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
}

void screen_move(int row, int col) {
    if (row < 0) row = 0;
    if (row >= SCREEN_ROWS) row = SCREEN_ROWS - 1;
    if (col < 0) col = 0;
    if (col >= SCREEN_COLS) col = SCREEN_COLS - 1;
    cur_row = row;
    cur_col = col;
    term_move(cur_row, cur_col);
}

void screen_put(cell_t cell) {
    printf("\033[3%dm\033[4%dm", CELL_FG(cell) % 8, CELL_BG(cell) % 8);
    char ch = CELL_CH(cell);
    if (ch < 32 || ch > 126) return;  // ignore non-printable
    putchar(CELL_CH(ch));
    cur_col++;
    if (cur_col >= SCREEN_COLS) {
        cur_col = 0;
        cur_row++;
        if (cur_row >= SCREEN_ROWS) cur_row = 0;
        term_move(cur_row, cur_col);
    }
}

void screen_clear(int x, int y, int w, int h, int color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCREEN_COLS) w = SCREEN_COLS - x;
    if (y + h > SCREEN_ROWS) h = SCREEN_ROWS - y;
    if (w <= 0 || h <= 0) return;

    for (int r = y; r < y + h; r++) {
        screen_move(r, x);
        for (int c = 0; c < w; c++) {
            screen_put(CELL(' ', ANSI_WHITE, color));
        }
    }
    screen_move(0, 0);
}
