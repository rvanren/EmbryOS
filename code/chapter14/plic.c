#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#define PLIC_ENABLE          (PLIC_BASE + 0x002080)
#define PLIC_PRIORITY(src)   (PLIC_BASE + 0x000000 + (src) * 0x4)
#define PLIC_ENABLE(ctx)     (PLIC_BASE + 0x002000 + (ctx) * 0x80)
#define PLIC_THRESHOLD(ctx)  (PLIC_BASE + 0x200000 + (ctx) * 0x1000)
#define PLIC_CLAIM(ctx)      (PLIC_BASE + 0x200004 + (ctx) * 0x1000)

#define HART1_CONTEXT   0
#define PLIC_H1_ENABLE       PLIC_ENABLE(HART1_CONTEXT)
#define PLIC_H1_THRESHOLD    PLIC_THRESHOLD(HART1_CONTEXT)
#define PLIC_H1_CLAIM        PLIC_CLAIM(HART1_CONTEXT)

#define PLIC_THRESHOLD     (PLIC_BASE + 0x200000)
#define PLIC_CLAIM         (PLIC_BASE + 0x201004)

#define MIE_MASK (1u << 3)

void plic_handler(struct trap_frame *tf) {
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM;
    if (claim == UART_IRQ) uart_isr();
    *(volatile uint32_t *)PLIC_CLAIM = claim;
}

void plic_init() {
    *(volatile uint32_t *)(PLIC_PRIORITY(0) + 4 * UART_IRQ) = 1;
    *(volatile uint32_t *)(PLIC_ENABLE) = (1 << UART_IRQ);
    *(volatile uint32_t *)(PLIC_THRESHOLD) = 0;
}
