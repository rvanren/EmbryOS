#include <stdint.h>
#include "interrupt.h"
#include "stdio.h"
#include "process.h"

#define PLIC_BASE   0x0C000000
#define PLIC_ENABLE (PLIC_BASE + 0x2080)   // enable bits for hart 0 M-mode
#define PLIC_PRIORITY (PLIC_BASE + 0x0000)
#define PLIC_THRESHOLD (PLIC_BASE + 0x200000)

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

    // Give UART interrupt (source 10) a nonzero priority
    *(volatile uint32_t *)(PLIC_PRIORITY + 4 * 10) = 1;
    // Enable UART interrupt for hart 0
    *(volatile uint32_t *)(PLIC_ENABLE) = (1 << 10);
    // Set threshold = 0 (accept all priorities)
    *(volatile uint32_t *)(PLIC_THRESHOLD) = 0;

    asm("csrs mie, %0" :: "r"(1 << 11)); // MEIE=1 → allow external interrupts
    asm("csrs mstatus, %0" :: "r"(1 << 3)); // MIE=1 → global interrupt enable
}
