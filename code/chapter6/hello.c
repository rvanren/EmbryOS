#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"
#include "mtime.h"
#include "sbi.h"

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    uint64_t now = mtime_get();
    sbi_set_timer(now + QUANTUM);
}

int main(void) {
    frame_init(); intr_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    plic_init();
    sched_init(proc_init((struct rect){ 0, 0, 80, 24 }));
    intr_set_handler(INTR_EXTERNAL, plic_handler);
    intr_set_handler(INTR_TIMER, timer_handler);
    uint64_t now = mtime_get();
    sbi_set_timer(now + QUANTUM);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
