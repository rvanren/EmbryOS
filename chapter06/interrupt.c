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
    void syscall_handler(struct trap_frame *tf);
    void exception_handler(struct trap_frame *tf);

    L3(L_FREQ, L_TRAP, tf->scause, tf->sepc, tf->stval);
    if (tf->scause & SCAUSE_INTERRUPT_MASK) // Interrupts
        switch (tf->scause & SCAUSE_CODE_MASK) {
            case 5:  timer_handler(tf); break;
            default: no_handler(tf);
        }
    else // Exceptions
        switch (tf->scause & SCAUSE_CODE_MASK) {
            case 8:  syscall_handler(tf); break;
            case 12: case 13: case 15:
                if (VM_START <= tf->stval && tf->stval < VM_END) {
                    proc_pagefault(tf);
                    break;
                }
                // FALL THROUGH
            default: exception_handler(tf);
        }

    // While we're at it check to see if input is available
    extern struct uart_info *uart;
    uart->isr(uart->base);
    sched_yield(); // Call the scheduler

    struct pcb *self = sched_self();
    if (self != 0 && self->executable > 0)
        vm_flush(self->hart, self->base);
}
