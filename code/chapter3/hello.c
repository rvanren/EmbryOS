#include "process.h"

/* crude busy-wait delay */
static void delay(void) {
    for (volatile int i = 0; i < 10000000; i++)
        ;
}

void taskA(void) {
    struct pcb *self = proc_self();
    int col = 0;
    while (1) {
        proc_put(self, 1, col, 'A', 2, 0);
        col = (col + 1) % self->area.w;
        delay();
        proc_yield();
    }
}

void taskB(void) {
    struct pcb *self = proc_self();
    int row = 0;
    while (1) {
        proc_put(self, row, 5, 'B', 4, 0);
        row = (row + 1) % self->area.h;
        delay();
        proc_yield();
    }
}

int main(void) {
    proc_init();
    proc_create(taskA,  0,  0, 40, 12);    // upper left
    proc_create(taskA, 40, 12, 40, 12);    // lower right
    proc_create(taskB, 40,  0, 40, 12);    // upper right
    proc_create(taskB,  0, 12, 40, 12);    // lower left

    while (1) proc_yield();
}
