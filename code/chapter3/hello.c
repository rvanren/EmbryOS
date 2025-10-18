#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "ctx.h"

static void delay(void) {
    for (volatile int i = 0; i < 100000; i++) sched_yield();
}

void taskA(void) {
    struct pcb *self = run_queue->next;
    int col = 0;
    while (1) {
        proc_put(self, 1, col, 'A', 2, 0);
        col = (col + 1) % self->area.w;
        delay();
    }
}

void taskB(void) {
    struct pcb *self = run_queue->next;
    int row = 0;
    while (1) {
        proc_put(self, row, 5, 'B', 4, 0);
        row = (row + 1) % self->area.h;
        delay();
    }
}

int main(void) {
    frame_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);

    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    sched_run(taskB, (struct rect){ 40,  0,  40, 12 });  // upper-right
    sched_run(taskB, (struct rect){ 0,  12,  40, 12 });  // lower-left
    sched_run(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    // Run the main loop
    for (;;) sched_yield();
}
