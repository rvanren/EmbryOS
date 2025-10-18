#include "stdio.h"
#include "process.h"
#include "clint.h"

#define MTIME_ADDR       (CLINT_BASE + 0xBFF8)
#define MTIME_CMP(hart)  (CLINT_BASE + 0x4000 + 8 * (hart))

#define MTIE_MASK        (1u << 7)

#define QUANTUM          100000         // 100 milliseconds

long long mtime_get() {
    return *((long long *) MTIME_ADDR);
}

void mtime_reset() {
    *((long long *) MTIME_CMP(1)) = mtime_get() + QUANTUM;
}

void timer_handler() {
    proc_yield();
    mtime_reset(); // add another quantum
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
    proc_init();
    clint_init();
    clint_set_handler(CLINT_TIMER, timer_handler());

    mtime_reset();
    asm("csrs mie, %0"::"r"(MTIE_MASK)); // set MTIE=1, unmask timer interrupts

    proc_create(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    proc_create(taskA, (struct rect){ 40,  0,  40, 12 });  // upper-right
    proc_create(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    proc_create(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    interrupts_enable();
    for (;;) {
        __asm__ volatile ("wfi");  // wait-for-interrupt
    }
}
