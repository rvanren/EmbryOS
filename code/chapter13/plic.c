#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#ifndef ORIG

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
    *(volatile uint32_t *)(PLIC_PRIORITY(UART_IRQ) = 1;
    *(volatile uint32_t *)(PLIC_ENABLE(HART_CTX) = (1 << UART_IRQ);
    *(volatile uint32_t *)(PLIC_THRESHOLD(HART_CTX)) = 0;
}

#else

#include <stdint.h>
#include "trap.h"     // for struct trap_frame, CAUSE_MACHINE_EXTERNAL_INTERRUPT, etc.
#include "uart.h"     // for uart_isr()

/* ----- Address calculation helpers ----- */

static inline uintptr_t plic_priority_addr(int source)
{
    return PLIC_BASE + 4 * source;
}

static inline uintptr_t plic_enable_addr(int context)
{
    return PLIC_BASE + 0x2000 + 0x80 * context;
}

static inline uintptr_t plic_threshold_addr(int context)
{
    return PLIC_BASE + 0x200000 + 0x1000 * context;
}

static inline uintptr_t plic_claim_addr(int context)
{
    return PLIC_BASE + 0x200004 + 0x1000 * context;
}

/* ---------------------------------------------------------------------
   Interrupt handler for a given context
   --------------------------------------------------------------------- */

void plic_handler(struct trap_frame *tf) {
    volatile uint32_t *claim_reg = (uint32_t *)plic_claim_addr(HART_CTX);
    uint32_t claim = *claim_reg;      // claim next pending interrupt

    switch (claim) {
    case UART_IRQ:
        uart_isr();
        break;

    // add more devices here
    // case SOME_OTHER_IRQ: ...
    //     device_isr();
    //     break;

    default:
        break;
    }

    *claim_reg = claim;               // complete the interrupt
}

void plic_init(void)
{
    // 1. Give this source a non-zero priority
    *(volatile uint32_t *)plic_priority_addr(UART_IRQ) = 1;

    // 2. Enable this source for this context
    *(volatile uint32_t *)plic_enable_addr(HART_CTX) = (1u << UART_IRQ);

    // 3. Accept all priorities ≥ 1
    *(volatile uint32_t *)plic_threshold_addr(HART_CTX) = 0;
}

#endif
