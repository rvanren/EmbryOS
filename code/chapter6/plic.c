#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#define PLIC_PRIORITY(src)   (PLIC_BASE + 4 * (src))
#define PLIC_THRESHOLD(ctx)  (PLIC_BASE + 0x200000 + 0x1000 * (ctx))
#define PLIC_CLAIM(ctx)      (PLIC_BASE + 0x200004 + 0x1000 * (ctx))
#define PLIC_ENABLE(ctx)     (PLIC_BASE + 0x002000 + 0x0080 * (ctx))

// WARNING: not guaranteed to be portable
#define PLIC_CTX(hart)       (2 * (hart) + 1)

static uint32_t hart_ctx;

void plic_handler(struct trap_frame *tf) {
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM(hart_ctx);
    if (claim == UART_IRQ) uart_isr();
    *(volatile uint32_t *)PLIC_CLAIM(hart_ctx) = claim;
}

void plic_init(uint32_t hartid) {
    hart_ctx = PLIC_CTX(hartid);
    // Give UART a non-zero priority
    *(volatile uint32_t *)(PLIC_PRIORITY(UART_IRQ))  = 1;
    // Enable UART for this context
    *(volatile uint32_t *)(PLIC_ENABLE(hart_ctx))    = (1 << UART_IRQ);
    // Accept all priorities > 0
    *(volatile uint32_t *)(PLIC_THRESHOLD(hart_ctx)) = 0;
}
