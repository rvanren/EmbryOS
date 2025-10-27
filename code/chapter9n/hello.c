#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"
#include "uart.h"
#include "plic.h"

#ifdef CH11
#include "files.h"
#endif

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    printf("trap: cause=%d mepc=%x mtval=%x<",
                        tf->mcause & 0xFFF, tf->mepc, tf->mtval);
    proc_exit();
}

int main(void) {
    extern void syscall_handler(struct trap_frame *);

    frame_init(); intr_init(); plic_init(); uart_init(); mtime_init(); pmp_init();

#ifdef CH11
    files_init();
#endif

    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    mtime_reset(QUANTUM);
    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
