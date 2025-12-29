#include "embryos.h"

void screen_put(int col, int row, cell_t cell) {
    if (col < 0 || col >= SCREEN_COLS) die("screen_put: bad column");
    if (row < 0 || row >= SCREEN_ROWS) die("screen_put: bad row");
    static int cur_row = -1, cur_col = -1;
    static int cur_fg = -1, cur_bg = -1;
    if (row != cur_row || col != cur_col + 1)
        kprintf("\033[%d;%dH", row + 1, col + 1);
    int t = CELL_T(cell);
    int fg = CELL_FG(cell) % 8, bg = CELL_BG(cell) % 8;
    char ch = CELL_CH(cell);
    if (fg == bg) { t = CELL_ASCII; ch = ' '; fg = (bg + 1) % 8; }
    if (fg != cur_fg) { kprintf("\033[3%dm", fg); cur_fg = fg; }
    if (bg != cur_bg) { kprintf("\033[4%dm", bg); cur_bg = bg; }
    switch (t) {
    case CELL_ASCII:
        if (ch < 32 || ch > 126) ch = '?';
        io_putchar(CELL_CH(ch));
        break;
    case CELL_BLOCK:
        switch (CELL_CH(ch) & 0x3) {
            case 0: io_putchar(' '); break;
            case 1: kprintf("\xE2\x96\x80"); break;
            case 2: kprintf("\xE2\x96\x84"); break;
            case 3: kprintf("\xE2\x96\x88"); break;
        }
        break;
    case CELL_BRAILLE:
        {
            unsigned char mask = CELL_CH(ch);
            unsigned char b2 = 0xA0 | (mask >> 6);     // top 2 bits
            unsigned char b3 = 0x80 | (mask & 0x3F);   // lower 6 bits
            io_putchar(0xE2); io_putchar(b2); io_putchar(b3);
        }
        break;
    default:
        io_putchar(CELL_CH('?'));
    }
    cur_row = row; cur_col = col;
}

void screen_fill(int x, int y, int w, int h, cell_t cell) {
    kprintf("\033[?25l");    // hide cursor (we simulate our own)

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCREEN_COLS) w = SCREEN_COLS - x;
    if (y + h > SCREEN_ROWS) h = SCREEN_ROWS - y;
    if (w <= 0 || h <= 0) return;

    for (int r = y; r < y + h; r++)
        for (int c = 0; c < w; c++) screen_put(c, r, cell);
}
