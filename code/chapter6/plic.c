#include "platform.h"
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"

#define PLIC_PRIORITY(src)   (PLIC_BASE + 4ULL * (src))
#define PLIC_THRESHOLD(ctx)  (PLIC_BASE + 0x200000ULL + 0x1000ULL * (ctx))
#define PLIC_CLAIM(ctx)      (PLIC_BASE + 0x200004ULL + 0x1000ULL * (ctx))
#define PLIC_ENABLE(ctx)     (PLIC_BASE + 0x002000ULL + 0x0080ULL * (ctx))

static uint32_t hart_ctx;

void plic_handler(struct trap_frame *tf) {
    volatile uint32_t *claim_reg =
        (volatile uint32_t *)(uintptr_t)PLIC_CLAIM(hart_ctx);
    uint32_t claim = *claim_reg;
    if (claim == UART_IRQ)
        uart_isr();
    *claim_reg = claim;
}

void plic_init(uint32_t hartid) {
    hart_ctx = PLIC_CTX(hartid);
    *(volatile uint32_t *)(uintptr_t)PLIC_PRIORITY(UART_IRQ)  = 1;
    *(volatile uint32_t *)(uintptr_t)PLIC_ENABLE(hart_ctx)    = (1u << UART_IRQ);
    *(volatile uint32_t *)(uintptr_t)PLIC_THRESHOLD(hart_ctx) = 0;
}
