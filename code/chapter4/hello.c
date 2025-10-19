#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "trap.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"
#include "syslib.h"
#include "uart.h"

#define QUANTUM          50000         // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void taskA(void) {
    for (int cnt = 0;; cnt++) {
        user_put(2 + cnt % 3, 2 + cnt % 3, 'A' + cnt % 26, cnt, 7 - cnt % 8);
        intr_enable();
        for (volatile int i = 0; i < 100000; i++) ;
        intr_disable();
    }
}

int main(void) {
    frame_init(); intr_init(); syscall_init(); uart_init(); mtime_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    mtime_reset(QUANTUM);

    sched_run(taskA, (struct rect){  0,  0, 40, 12 });  // upper-left
    sched_run(taskA, (struct rect){ 40,  0, 40, 12 });  // upper-right
    sched_run(taskA, (struct rect){  0, 12, 40, 12 });  // lower-left
    sched_run(taskA, (struct rect){ 40, 12, 40, 12 });  // lower-left

    sched_idle();
}
