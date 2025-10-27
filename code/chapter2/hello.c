#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "uart.h"
#include "screen.h"

int main(void) {
    uart_init();

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    extern void splash_main();
    splash_main();
}
