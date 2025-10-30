#include "screen.h"

int main(void) {
    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    extern void splash_main();
    splash_main();
}
