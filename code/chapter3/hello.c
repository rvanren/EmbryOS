#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "ctx.h"
#include "uart.h"

void taskA(void) {
    struct pcb *self = run_queue->next;
    int col = 0;
    while (1) {
        proc_put(self, 1, col, 'A', 2, 0);
        col = (col + 1) % self->area.w;
        for (volatile int i = 0; i < 100000; i++) sched_yield();
    }
}

void taskB(void) {
    struct pcb *self = run_queue->next;
    int row = 0;
    while (1) {
        proc_put(self, row, 5, 'B', 4, 0);
        row = (row + 1) % self->area.h;
        for (volatile int i = 0; i < 100000; i++) sched_yield();
    }
}

int main(void) {
    frame_init(); uart_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);

    sched_run(taskA, (struct rect){  0,  0, 40, 12 });  // upper-left
    sched_run(taskB, (struct rect){ 40,  0, 40, 12 });  // upper-right
    sched_run(taskB, (struct rect){  0, 12, 40, 12 });  // lower-left
    sched_run(taskA, (struct rect){ 40, 12, 40, 12 });  // lower-left

    sched_idle();
}
