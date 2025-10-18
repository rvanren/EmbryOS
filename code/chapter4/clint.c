#include "stdio.h"
#include "process.h"
#include "clint.h"

#define MIE_MASK (1u << 3)

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

void software_trap_handler() {
    int mcause, mepc;
    asm("csrr %0, mcause":"=r"(mcause));
    asm("csrr %0, mepc":"=r"(mepc));

    if (mcause & (1 << 31)) {   // interrupt?
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
    void _trap_handler();
    asm("csrw mtvec, %0"::"r"(_trap_handler));
}
