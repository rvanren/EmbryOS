#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#define PLIC_PRIORITY(src)   (PLIC_BASE + 4 * (src))
#define PLIC_ENABLE(ctx)     (PLIC_BASE + 0x002000 + 0x0080 * (ctx))
#define PLIC_THRESHOLD(ctx)  (PLIC_BASE + 0x200000 + 0x1000 * (ctx))
#define PLIC_CLAIM(ctx)      (PLIC_BASE + 0x200004 + 0x1000 * (ctx))

void plic_handler(struct trap_frame *tf) {
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM(HART_CTX);
    if (claim == UART_IRQ) uart_isr();
    *(volatile uint32_t *)PLIC_CLAIM(HART_CTX) = claim;
}

void plic_init() {
    // Give this source a non-zero priority
    *(volatile uint32_t *)(PLIC_PRIORITY(UART_IRQ) = 1;
    // Enable this source for this context
    *(volatile uint32_t *)(PLIC_ENABLE(HART_CTX) = (1 << UART_IRQ);
    // Accept all priorities ≥ 1
    *(volatile uint32_t *)(PLIC_THRESHOLD(HART_CTX)) = 0;
}
