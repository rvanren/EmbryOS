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

extern void (*applications[])();

#define QUANTUM          50000         // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

int main(void) {
    extern void user_setup(void);

    frame_init(); intr_init(); plic_init(); uart_init(); mtime_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    mtime_reset(QUANTUM);
    user_setup();

    sched_run(applications[0], (struct rect){ 0,   0,  40, 12 });  // upper-left
    sched_run(applications[1], (struct rect){ 40,  0,  40, 12 });  // upper-right
    sched_run(applications[0], (struct rect){ 0,  12,  40, 12 });  // lower-left

    sched_idle();
}
