#include "stdio.h"
#include "process.h"

#define CLINT_BASE       0x02000000UL
#define MTIME_ADDR       (CLINT_BASE + 0xBFF8)
#define MTIME_CMP(hart)  (CLINT_BASE + 0x4000 + 8 * (hart))

#define MTIE_MASK        (1u << 7)
#define MIE_MASK         (1u << 3)

#define QUANTUM          100000         // 100 milliseconds

long long mtime_get() {
    return *((long long *) MTIME_ADDR);
}

void mtime_reset() {
    *((long long *) MTIME_CMP(1)) = mtime_get() + QUANTUM;
}

static inline void interrupts_enable(void) {
    __asm__ volatile ("csrs mstatus, %0" :: "r"(MIE_MASK));
}

static inline void interrupts_disable(void) {
    __asm__ volatile ("csrc mstatus, %0" :: "r"(MIE_MASK));
}

void _trap_handler();

void handle_syscall() {
    printf("SYSCALL\n");
}

int is_interrupt(int mcause) {
    // most significant bit in `mcause` is set only when there is an interrupt
    return mcause & (1 << 31);
}

void software_trap_handler() {
    int mcause, mepc;
    asm("csrr %0, mcause":"=r"(mcause));
    asm("csrr %0, mepc":"=r"(mepc));

    if (is_interrupt(mcause)) {
        proc_yield();
        mtime_reset(); // add another quantum
    }
    else {
        printf("Exception %x\n", mcause);
        for (;;) ;
    }

    asm("csrw mepc, %0"::"r"(mepc));
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

    asm("csrw mtvec, %0"::"r"(_trap_handler));
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
