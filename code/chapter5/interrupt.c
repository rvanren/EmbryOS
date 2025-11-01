#include "platform.h"
#include "trap.h"
#include "interrupt.h"
#include "sched.h"
#include "kprintf.h"

#define SCAUSE_INTERRUPT_MASK  ((uword_t)1 << (__riscv_xlen - 1))
#define SCAUSE_CODE_MASK       ((uword_t)0xFFF)

static void no_handler(struct trap_frame *tf) {
    kprintf("Bad interrupt: scause=%X sepc=%X\n", tf->scause, tf->sepc);
    for (;;) ;
}

static trap_entry_t handlers[] = { no_handler, no_handler, no_handler, no_handler };

void software_trap_handler(struct trap_frame *tf) {
    if (tf->scause & SCAUSE_INTERRUPT_MASK) { // Interrupts
        switch (tf->scause & SCAUSE_CODE_MASK) {
        case 5: (*handlers[INTR_TIMER])(tf); break;      // supervisor timer
        case 9: (*handlers[INTR_EXTERNAL])(tf); break;   // supervisor external
        default: no_handler(tf);
        }
    } else { // Exceptions (includes system calls)
        switch (tf->scause & SCAUSE_CODE_MASK) {
        case 8: case 11:                                 // environment call
            (*handlers[INTR_SYSCALL])(tf);
            tf->sepc += 4;                               // skip ecall
            break;
        default: (*handlers[INTR_EXCEPTION])(tf);
        }
    }
}

void intr_set_handler(enum intr_class which, trap_entry_t handler) {
    handlers[which] = handler;
}

int intr_init(void) {
    extern void _trap_handler(void);
    asm volatile ("csrw stvec, %0" :: "r"((uword_t)_trap_handler));
    asm volatile ("csrs sie, %0" :: "r"((uword_t)1 << 9));
    asm volatile ("csrs sie, %0" :: "r"((uword_t)1 << 5));
    asm volatile ("csrs sstatus, %0" :: "r"((uword_t)1 << 1));
    return 0;
}
