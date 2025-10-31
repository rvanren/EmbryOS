#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"
#include "io.h"
#include "mtime.h"
#include "sbi.h"
#include "plic.h"
#include "uart.h"

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    uint64_t now = mtime_get();
    sbi_set_timer(now + QUANTUM);
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%d sepc=%x stval=%x<",
                        tf->scause & 0xFFF, tf->sepc, tf->stval);
    sched_exit();
}

void main(uint32_t hartid, uint32_t dtb_pa) {
    frame_init(); intr_init(); plic_init(hartid); uart_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
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
