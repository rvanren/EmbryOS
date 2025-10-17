#include "stdio.h"
#include "screen.h"

static int cur_row = 0;
static int cur_col = 0;

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

void screen_put(char ch, int color) {
    if (ch < 32 || ch > 126) return;  // ignore non-printable
    printf("\033[3%dm%c", color % 8, ch);
    cur_col++;
    if (cur_col >= SCREEN_COLS) {
        cur_col = 0;
        cur_row++;
        if (cur_row >= SCREEN_ROWS) cur_row = 0;
        term_move(cur_row, cur_col);
    }
}

void screen_clear(void) {
    for (int r = 0; r < SCREEN_ROWS; r++) {
        screen_move(r, 0);
        for (int c = 0; c < SCREEN_COLS; c++) {
            screen_put(' ', 7);   // 7 = white (default background)
        }
    }
    screen_move(0, 0);
}
