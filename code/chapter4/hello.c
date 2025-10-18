#include "frame.h"
#include "stdio.h"
#include "process.h"
#include "clint.h"
#include "mtime.h"

#define QUANTUM          100000         // 100 milliseconds

void timer_handler() {
    proc_yield();
    mtime_reset(QUANTUM); // add another quantum
}

static void delay(void) {
    interrupts_enable();
    for (volatile int i = 0; i < 100000; i++)
        ;
    interrupts_disable();
}

void taskA(void) {
    struct pcb *self = proc_self();
    for (int cnt = 0;; cnt++) {
        proc_put(self, 0, 0, 'A', 2, 0);
        printf(" %x %d", self, cnt);
        delay();
    }
}

int main(void) {
    frame_init();
    proc_init();
    clint_init();
    clint_set_handler(CLINT_TIMER, timer_handler);

    mtime_init();
    mtime_reset(QUANTUM);

    proc_create(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    proc_create(taskA, (struct rect){ 40,  0,  40, 12 });  // upper-right
    proc_create(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    proc_create(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    interrupts_enable();
    for (;;) {
        __asm__ volatile ("wfi");  // wait-for-interrupt
    }
}
