#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"

#define QUANTUM          50000         // 50 milliseconds

static inline void user_put(int row, int col, char c, int fg, int bg) {
    register int a7 asm("a7") = SYS_PUT;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = c;
    register int a3 asm("a3") = fg;
    register int a4 asm("a4") = bg;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a7));
}


void timer_handler() {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

static void delay(void) {
    intr_enable();
    for (volatile int i = 0; i < 100000; i++) ;
    intr_disable();
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    for (int cnt = 0;; cnt++) {
        user_put(2 + cnt % 3, 2 + cnt % 3, 'A' + cnt % 26, cnt, 7 - cnt % 8);
        delay();
    }
}

int main(void) {
    frame_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_init();
    intr_set_handler(INTR_TIMER, timer_handler);
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
    intr_enable();
    for (;;) __asm__ volatile ("wfi");  // wait-for-interrupt
}
