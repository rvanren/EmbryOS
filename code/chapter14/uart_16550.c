#include <stdint.h>
#include "platform.h"
#include "uart.h"
#include "io.h"

#ifdef UART_16550

#define UART_RBR    0x00  // Receiver Buffer Register (read)
#define UART_THR    0x00  // Transmit Holding Register (write)
#define UART_LSR    0x05  // Line Status Register
#define UART_IER    0x01  // Interrupt Enable Register

#define LSR_DATA_READY 0x01
#define LSR_TX_EMPTY   0x20

static volatile uint8_t *UART = (volatile uint8_t *) UART_BASE;

void uart_init(void) {
    UART[UART_IER] = 0x01;  // enable received data interrupt
}

void uart_putchar(char c) {
    while ((UART[UART_LSR] & LSR_TX_EMPTY) == 0) ;
    UART[UART_THR] = c;
}

void uart_isr(void) {
    for (;;) {
        if ((UART[UART_LSR] & LSR_DATA_READY) == 0) break;
        io_received(UART[UART_RBR]);
    }
}

#endif
