#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "uart.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"

void exception_handler(struct trap_frame *tf) {
    kprintf("trap: cause=%d mepc=%x mtval=%x<",
                        tf->mcause & 0xFFF, tf->mepc, tf->mtval);
    for (;;) ;
}

int main(void) {
    frame_init(); uart_init(); intr_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    extern void splash_main();
    splash_main();
}
