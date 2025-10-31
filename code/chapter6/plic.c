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

static inline uint32_t read_hartid(void) {
    uint32_t hartid;
    asm volatile ("csrr %0, mhartid" : "=r"(hartid));
    return hartid;
}

void plic_handler(struct trap_frame *tf) {
    uint32_t ctx = read_hartid;
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM(ctx);
    if (claim == UART_IRQ) uart_isr();
    *(volatile uint32_t *)PLIC_CLAIM(ctx) = claim;
}

void plic_init() {
    uint32_t ctx = read_hartid;
    // Give UART a non-zero priority
    *(volatile uint32_t *)(PLIC_PRIORITY(UART_IRQ)) = 1;
    // Enable UART for this context
    *(volatile uint32_t *)(PLIC_ENABLE(ctx))        = (1 << UART_IRQ);
    // Accept all priorities > 0
    *(volatile uint32_t *)(PLIC_THRESHOLD(ctx))     = 0;
}
