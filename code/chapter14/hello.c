#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "uart.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"
#include "plic.h"
#include "pmp.h"
#include "vm.h"
#include "files.h"

#include "mtime.h"

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%d mepc=%x mtval=%x<",
                        tf->mcause & 0xFFF, tf->mepc, tf->mtval);
    sched_exit();
}

int main(void) {
    frame_init(); intr_init(); uart_init(); plic_init(); pmp_init(); vm_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    sched_init(proc_init((struct rect){ 0, 0, 80, 24 }));

    extern void syscall_handler(struct trap_frame *);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, plic_handler);
    mtime_init();
    intr_set_handler(INTR_TIMER, timer_handler);
    mtime_reset(QUANTUM);
    files_init();

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
