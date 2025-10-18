#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "ctx.h"

static void delay(void) {
    for (volatile int i = 0; i < 100000; i++) sched_yield();
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    for (int cnt = 0;; cnt++) {
        proc_put(self, 2 + cnt % 3, 2 + cnt % 3, 'A' + cnt % 26,
                                cnt, 7 - cnt % 8);
        delay();
    }
}

int main(void) {
    frame_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);

    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    sched_run(taskA, (struct rect){ 40,  0,  40, 12 });  // upper-right
    sched_run(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    sched_run(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    // Run the main loop
    for (;;) sched_yield();
}
