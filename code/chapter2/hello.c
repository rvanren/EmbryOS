#include "kprintf.h"
#include "syscall.h"
#include "screen.h"
#include "frame.h"

int main(void) {
    frame_init();

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    extern void splash_main();
    splash_main();
}
