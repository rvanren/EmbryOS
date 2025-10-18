#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "stdio.h"
#include "process.h"
#include "kbd.h"

#define UART_IRQ    4

#define PLIC_BASE   0x0C000000
#define PLIC_ENABLE (PLIC_BASE + 0x2080)   // enable bits for hart 0 M-mode
#define PLIC_PRIORITY (PLIC_BASE + 0x0000)
#define PLIC_THRESHOLD (PLIC_BASE + 0x200000)
#define PLIC_CLAIM     (PLIC_BASE + 0x201004)

#define MIE_MASK (1u << 3)

void interrupt_handler(struct trap_frame *tf) {
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM;
    if (claim == UART_IRQ)
        kbd_isr(tf);
    *(volatile uint32_t *)PLIC_CLAIM = claim; // complete
}

static void no_handler(struct trap_frame *tf) {
    printf("Bad interrupt\n");
    for (;;) ;
}

static trap_entry_t handlers[] = {
    no_handler,     // timer
    no_handler,     // external
    no_handler,     // syscall
    no_handler      // exception
};

void intr_enable(void) {
    __asm__ volatile ("csrs mstatus, %0" :: "r"(MIE_MASK));
}

void intr_disable(void) {
    __asm__ volatile ("csrc mstatus, %0" :: "r"(MIE_MASK));
}

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
        default: (*handlers[INTR_EXCEPTION])(tf);
        }
    }
}

void intr_set_handler(int which, trap_entry_t handler) {
    handlers[which] = handler;
}

int intr_init() {
    void _trap_handler();
    asm("csrw mtvec, %0"::"r"(_trap_handler));

    // Give UART interrupt a nonzero priority
    *(volatile uint32_t *)(PLIC_PRIORITY + 4 * UART_IRQ) = 1;
    // Enable UART interrupt for hart 0
    *(volatile uint32_t *)(PLIC_ENABLE) = (1 << UART_IRQ);
    // Set threshold = 0 (accept all priorities)
    *(volatile uint32_t *)(PLIC_THRESHOLD) = 0;

    asm("csrs mie, %0" :: "r"(1 << 11)); // MEIE=1 → allow external interrupts
}
