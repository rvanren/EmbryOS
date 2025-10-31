#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "sched.h"
#include "kprintf.h"

static void no_handler(struct trap_frame *tf) {
    kprintf("Bad interrupt: scause=%x sepc=%x\n", tf->scause, tf->sepc);
    for (;;) ;
}

static trap_entry_t handlers[] = { no_handler, no_handler, no_handler, no_handler };

void software_trap_handler(struct trap_frame *tf) {
    int scause, sepc;
    asm("csrr %0, scause":"=r"(scause));

    if (scause & (1 << 31)) {   // interrupt?
        switch (scause & 0xFFF) {
        case 5: (*handlers[INTR_TIMER])(tf); break;
        case 9: (*handlers[INTR_EXTERNAL])(tf); break;
        default: kprintf("Unknown interrupt cause %x\n", scause);
        }
    }
    else {
        switch (scause & 0xFFF) {
        case 8: case 11: (*handlers[INTR_SYSCALL])(tf); tf->sepc += 4; break;
        default:
            (*handlers[INTR_EXCEPTION])(tf);
        }
    }
}

void intr_set_handler(enum intr_class which, trap_entry_t handler) {
    handlers[which] = handler;
}

#define SSTATUS_SIE  (1 << 1)
#define SIE_STIE     (1 << 5)	// Supervisor timer interrupts
#define SIE_SEIE     (1 << 9)	// Supervisor external interrupts (UART)

int intr_init() {
    void _trap_handler();
    uintptr_t handler = (uintptr_t) _trap_handler;
    asm volatile ("csrw stvec, %0" :: "r"(handler));  // Set S-mode trap vector
    asm volatile ("csrs sie, %0" :: "r"(SIE_STIE | SIE_SEIE));  // per-source
    asm volatile ("csrs sstatus, %0" :: "r"(SSTATUS_SIE));      // global
}
