#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"
#include "syslib.h"
#include "uart.h"
#include "plic.h"

#define QUANTUM          50000         // 50 milliseconds

extern void (*applications[])();

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, 0, 0, '>', 0, 1);
    printf("trap: cause=%d mepc=%x mtval=%x   \n",
                            mcause & 0xFFF, tf->mepc, tf->mtval);
    proc_exit();
}

int main(void) {
    frame_init(); intr_init(); plic_init(); uart_init(); mtime_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    mtime_reset(QUANTUM);
    sched_run(applications[0], (struct rect){ 0,   0,  40, 12 });
    sched_idle();
}
