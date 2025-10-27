#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "sched.h"
#include "stdio.h"

#ifdef CH10
#include "pmp.h"
#endif

#define MIE_MASK (1u << 3)

static void no_handler(struct trap_frame *tf) {
    printf("Bad interrupt\n");
    for (;;) ;
}

static trap_entry_t handlers[] = { no_handler, no_handler, no_handler, no_handler };

void intr_enable(void) { __asm__ volatile ("csrs mstatus, %0" :: "r"(MIE_MASK)); }
void intr_disable(void) { __asm__ volatile ("csrc mstatus, %0" :: "r"(MIE_MASK)); }

void software_trap_handler(struct trap_frame *tf) {
    int mcause, mepc;
    asm("csrr %0, mcause":"=r"(mcause));
    if (mcause & (1 << 31)) {   // interrupt?
        switch (mcause & 0xFFF) {
        case  3: break;
        case  7: (*handlers[INTR_TIMER])(tf); break;
        case 11: (*handlers[INTR_EXTERNAL])(tf); break;
        default: printf("Unknown interrupt cause %x\n", mcause);
        }
    }
    else {
        switch (mcause & 0xFFF) {
        case 8: case 11: (*handlers[INTR_SYSCALL])(tf); tf->mepc += 4; break;
        default:
            (*handlers[INTR_EXCEPTION])(tf);
        }
    }

    struct pcb *self = run_queue[proc_current]->next;
#ifdef CH10
    if (self->base != 0) pmp_load(self);
#endif
}

void intr_set_handler(enum intr_class which, trap_entry_t handler) {
    handlers[which] = handler;
}

int intr_init() {
    void _trap_handler();
    asm("csrw mtvec, %0"::"r"(_trap_handler));
    asm("csrs mie, %0" :: "r"(1 << 11)); // MEIE=1 -> allow external interrupts

#ifndef CH10
    user_setup();
#endif
}
