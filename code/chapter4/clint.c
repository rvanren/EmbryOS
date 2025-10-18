#include "stdio.h"
#include "process.h"
#include "clint.h"

#define MIE_MASK         (1u << 3)

static void no_handler() {
    printf("Bad interrupt\n");
    for (;;) ;
}

static entry_t handlers[] = {
    no_handler,     // timer
    no_handler,
    no_handler,
    no_handler
};

void interrupts_enable(void) {
    __asm__ volatile ("csrs mstatus, %0" :: "r"(MIE_MASK));
}

void interrupts_disable(void) {
    __asm__ volatile ("csrc mstatus, %0" :: "r"(MIE_MASK));
}

void _trap_handler();

static int is_interrupt(int mcause) {
    // most significant bit in `mcause` is set only when there is an interrupt
    return mcause & (1 << 31);
}

void software_trap_handler() {
    int mcause, mepc;
    asm("csrr %0, mcause":"=r"(mcause));
    asm("csrr %0, mepc":"=r"(mepc));

    if (is_interrupt(mcause)) {
        (*handlers[CLINT_TIMER])();
    }
    else {
        printf("Exception %x\n", mcause);
        for (;;) ;
    }

    asm("csrw mepc, %0"::"r"(mepc));
}

void clint_set_handler(int which, entry_t handler){
    handlers[which] = handler;
}

int clint_init(){
    asm("csrw mtvec, %0"::"r"(_trap_handler));
}
