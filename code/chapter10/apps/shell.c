#include "syslib.h"

#define NROWS 12
#define NCOLS 40

struct cell { char c, fg, bg; };
struct cell screen[NROWS][NCOLS];
int cur_col = 0, cur_fg = 0, cur_bg = 7;

void screen_init(){
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen[row][col] = (struct cell) { ' ', 0, 7 };
        }
    }
}

void screen_sync(){
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            user_put(row, col, screen[row][col].c, screen[row][col].fg, screen[row][col].bg);
        }
    }
}

void scroll(){
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

void putchar(char c){
    if (c == '\r' || cur_col == NCOLS) {
        scroll();
        cur_col = 0;
    }
    if (c != '\r') {
        user_put(NROWS - 1, cur_col, c, cur_fg, cur_bg);
        screen[NROWS - 1][cur_col] = (struct cell) { c, cur_fg, cur_bg };
        cur_col++;
    }
}

void main(void) {
    screen_init();
    screen_sync();
    putchar('$'); putchar(' ');
    for (;;) {
        char c = user_get();
        putchar(c);
        if (c == '1') user_spawn(1, 40,  0, 40, 12);
        if (c == '2') user_spawn(1, 40, 12, 40, 12);
        if (c == '3') user_spawn(1,  0, 12, 40, 12);
        if (c == '!') user_spawn(2,  0,  0, 40, 12);
        if (c == '.') user_exit();
    }
}
