#include "interrupt.h"
#include "stdio.h"
#include "process.h"

#define MIE_MASK (1u << 3)

static void no_handler() {
    printf("Bad interrupt\n");
    for (;;) ;
}

static entry_t handlers[] = {
    no_handler,     // timer
    no_handler,     // external
    no_handler,     // syscall
    no_handler      // exception
};

void intr_enable(void) {
    __asm__ volatile ("csrs mstatus, %0" :: "r"(MIE_MASK));
}

void software_trap_handler() {
    int mcause, mepc;
    asm("csrr %0, mcause":"=r"(mcause));
    asm("csrr %0, mepc":"=r"(mepc));
    if (mcause & (1 << 31)) {   // interrupt?
        switch (mcause & 0xFFF) {
        case  3: break;
        case  7: (*handlers[INTR_TIMER])(); break;
        case 11: (*handlers[INTR_EXTERNAL])(); break;
        default: printf("Unknown interrupt cause %x\n", mcause);
        }
    }
    else {
        switch (mcause & 0xFFF) {
        case 8: case 11: (*handlers[INTR_SYSCALL])(); mepc += 4; break;
        default: (*handlers[INTR_EXCEPTION])();
        }
    }
    asm("csrw mepc, %0"::"r"(mepc));
}


void intr_set_handler(int which, entry_t handler) {
    handlers[which] = handler;
}

int intr_init() {
    void _trap_handler();
    asm("csrw mtvec, %0"::"r"(_trap_handler));
}
