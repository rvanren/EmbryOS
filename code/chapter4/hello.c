#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "clint.h"
#include "ctx.h"
#include "mtime.h"

#define QUANTUM          100000         // 100 milliseconds

void timer_handler() {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

static void delay(void) {
    interrupts_enable();
    for (volatile int i = 0; i < 100000; i++) ;
    interrupts_disable();
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    for (int cnt = 0;; cnt++) {
        proc_put(self, 0, 0, 'A' + cnt % 26, cnt, 7 - cnt % 8);
        delay();
    }
}

int main(void) {
    frame_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    clint_init();
    clint_set_handler(CLINT_TIMER, timer_handler);
    mtime_init();
    mtime_reset(QUANTUM);

    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    sched_run(taskA, (struct rect){ 40,  0,  40, 12 });  // upper-right
    sched_run(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    sched_run(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    // Switch priority to level 2
    proc_dequeue(&run_queue[proc_current]);
    proc_enqueue(&run_queue[2], pcb);
    proc_current = 2;
    sched_yield();

    // Run the main loop, which is waiting for interrupts at lowest priority
    interrupts_enable();
    for (;;) __asm__ volatile ("wfi");  // wait-for-interrupt
}
