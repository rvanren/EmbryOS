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

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%d mepc=%x mtval=%x<",
                        tf->mcause & 0xFFF, tf->mepc, tf->mtval);
    proc_exit();
}

int main(void) {
    frame_init(); intr_init(); uart_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    plic_init();
    pmp_init();

    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);

    extern void syscall_handler(struct trap_frame *);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, plic_handler);
    mtime_init();
    intr_set_handler(INTR_TIMER, timer_handler);
    mtime_reset(QUANTUM);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
