#include <stdint.h>
#include "trap.h"
#include "interrupt.h"
#include "stdio.h"
#include "process.h"
#include "uart.h"
#include "plic.h"
#include "platform.h"

#define PLIC_ENABLE     (PLIC_BASE + 0x2080)      // enable bits for hart 0 M-mode
#define PLIC_PRIORITY   (PLIC_BASE + 0x0000)
#define PLIC_THRESHOLD  (PLIC_BASE + 0x200000)
#define PLIC_CLAIM      (PLIC_BASE + 0x201004)

#define MIE_MASK (1u << 3)

void interrupt_handler(struct trap_frame *tf) {
    uint32_t claim = *(volatile uint32_t *)PLIC_CLAIM;
    if (claim == UART_IRQ) uart_isr(tf);
    *(volatile uint32_t *)PLIC_CLAIM = claim;
}

int plic_init() {
    *(volatile uint32_t *)(PLIC_PRIORITY + 4 * UART_IRQ) = 1;
    *(volatile uint32_t *)(PLIC_ENABLE) = (1 << UART_IRQ);
    *(volatile uint32_t *)(PLIC_THRESHOLD) = 0;
}
