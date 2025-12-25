#include "embryos.h"

#define REG(base, index) (*(volatile uint32_t *)((base) + ((index) << 2)))

#define UART_RBR   0   // RX buffer
#define UART_THR   0   // TX hold
#define UART_IER   1
#define UART_IIR   2
#define UART_FCR   2
#define UART_LCR   3
#define UART_MCR   4
#define UART_LSR   5

#define LSR_DR     (1 << 0)
#define LSR_THRE   (1 << 5)
#define LSR_TEMT   (1 << 6)

void uart_pxa_wait_tx_empty(uintptr_t base) {
    while (!(REG(base, UART_LSR) & LSR_THRE)) ;
    while (!(REG(base, UART_LSR) & LSR_TEMT)) ;
}

void uart_pxa_putchar(uintptr_t base, int c) {
    uart_pxa_wait_tx_empty(base);
    REG(base, UART_THR) = (uint32_t)c;
}

void uart_pxa_init(uintptr_t base) {
    uart_pxa_wait_tx_empty(base);   // to prevent TX buffer erase

    // Enable FIFO + clear RX/TX FIFOs
    REG(base, UART_FCR) = (1<<0) | (1<<1) | (1<<2);

    // Ensure 8n1 format (U-Boot probably already set this)
    REG(base, UART_LCR) = 0x03;
}

void uart_pxa_isr(uintptr_t base) {
    while (REG(base, UART_LSR) & LSR_DR)
        io_received(REG(base, UART_RBR) & 0xff);
}
