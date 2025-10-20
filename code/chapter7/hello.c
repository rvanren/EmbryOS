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

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void taskA(void) {
    for (int cnt = 0;; cnt++) {
        user_put(2 + cnt % 3, 2 + cnt % 3, 'A' + cnt % 26, cnt, 7 - cnt % 8);
        for (volatile int i = 0; i < 100000; i++) ;
    }
}

void taskB(void) {
    user_put(10, 3, '$', 2, 0);
    for (int counter = 0;; counter++) {
        char c = user_get();
        user_put(10, 5 + counter % 10, c, 2, 0);
        if (c == '!') user_spawn(0, 40, 12, 40, 12);
        if (c == '.') user_exit();
    }
}

void (*applications[])() = { taskA, taskB };
int n_applications = sizeof(applications) / sizeof(applications[0]);

int main(void) {
    frame_init(); intr_init(); plic_init(); syscall_init(); uart_init(); mtime_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    mtime_reset(QUANTUM);
    ctx_user_setup();

    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    sched_run(taskB, (struct rect){ 40,  0,  40, 12 });  // upper-right
    sched_run(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left

    sched_idle();
}
