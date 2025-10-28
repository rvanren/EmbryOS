#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#ifdef ORIG

#define PLIC_ENABLE     (PLIC_BASE + 0x2080)      // enable bits for hart 0 M-mode
#define PLIC_PRIORITY   (PLIC_BASE + 0x0000)
#define PLIC_THRESHOLD  (PLIC_BASE + 0x200000)
#define PLIC_CLAIM      (PLIC_BASE + 0x201004)

void plic_handler(struct trap_frame *tf) {
    kprintf("X");
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM;
    if (claim == UART_IRQ) uart_isr();
    *(volatile uint32_t *)PLIC_CLAIM = claim;
}

void plic_init() {
    *(volatile uint32_t *)(PLIC_PRIORITY + 4 * UART_IRQ) = 1;
    *(volatile uint32_t *)(PLIC_ENABLE) = (1 << UART_IRQ);
    *(volatile uint32_t *)(PLIC_THRESHOLD) = 0;
}

#endif

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
    volatile uint32_t *claim_reg = (uint32_t *)plic_claim_addr(1);
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
    *(volatile uint32_t *)plic_enable_addr(HART_CTX) |= (1u << UART_IRQ);

    // 3. Accept all priorities ≥ 1
    *(volatile uint32_t *)plic_threshold_addr(HART_CTX) = 0;
}
