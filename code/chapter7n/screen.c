#include "stdio.h"
#include "kprintf.h"
#include "uart.h"

void screen_put(int row, int col, cell_t cell) {
    if (row < 0) row = 0;
    if (row >= SCREEN_ROWS) row = SCREEN_ROWS - 1;
    if (col < 0) col = 0;
    if (col >= SCREEN_COLS) col = SCREEN_COLS - 1;
    kprintf("\033[%d;%dH", row + 1, col + 1);
    kprintf("\033[3%dm\033[4%dm", CELL_FG(cell) % 8, CELL_BG(cell) % 8);
    char ch = CELL_CH(cell);
    if (ch < 32 || ch > 126) return;  // ignore non-printable
    uart_putchar(CELL_CH(ch));
}

void screen_fill(int x, int y, int w, int h, cell_t cell) {
    kprintf("\033[?25l");    // hide cursor (we simulate our own)

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCREEN_COLS) w = SCREEN_COLS - x;
    if (y + h > SCREEN_ROWS) h = SCREEN_ROWS - y;
    if (w <= 0 || h <= 0) return;

    for (int r = y; r < y + h; r++)
        for (int c = 0; c < w; c++) screen_put(r, c, cell);
}
