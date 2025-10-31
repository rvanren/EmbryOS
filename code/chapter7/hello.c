#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "uart.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"
#include "plic.h"
#include "pmp.h"
#include "mtime.h"
#include "sbi.h"

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    sbi_set_timer(mtime_get() + QUANTUM);
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%d sepc=%x stval=%x<",
                        tf->scause & 0xFFF, tf->sepc, tf->stval);
    sched_exit();
}

void main(uint32_t hartid, uint32_t dtb_pa) {
    frame_init(); intr_init(); uart_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    plic_init(hartid); pmp_init();
    sched_init(proc_init((struct rect){ 0, 0, 80, 24 }));

    extern void syscall_handler(struct trap_frame *);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, plic_handler);
    intr_set_handler(INTR_TIMER, timer_handler);
    sbi_set_timer(mtime_get() + QUANTUM);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
