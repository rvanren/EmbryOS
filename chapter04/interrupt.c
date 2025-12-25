#include "embryos.h"

#define SCAUSE_INTERRUPT_MASK  ((uword_t)1 << (__riscv_xlen - 1))
#define SCAUSE_CODE_MASK       ((uword_t)0xFFF)

static void no_handler(struct trap_frame *tf) {
    kprintf("Bad interrupt: scause=%X sepc=%X stval=%X\n",
                                tf->scause, tf->sepc, tf->stval);
    for (;;) ;
}

void software_trap_handler(struct trap_frame *tf) {
    void timer_handler(struct trap_frame *tf);
    void exception_handler(struct trap_frame *tf);

    L3(L_FREQ, L_TRAP, tf->scause, tf->sepc, tf->stval);
    if (tf->scause & SCAUSE_INTERRUPT_MASK) { // Interrupts
        switch (tf->scause & SCAUSE_CODE_MASK) {
            case 5:  timer_handler(tf); break;
            default: no_handler(tf);
        }
    }
    else // Exceptions (includes system calls)
        exception_handler(tf);

    sched_yield(); // Call the scheduler
}
