#include <stdint.h>
#include "uart.h"
#include "io.h"
#include "platform.h"

#ifdef UART_16550

#define UART_THR    0x00  // Transmit Holding Register (write)
#define UART_LSR    0x05  // Line Status Register

#define LSR_TX_EMPTY   0x20

static volatile uint8_t *UART = (volatile uint8_t *) UART_BASE;

void uart_init(void) {
}

void uart_putchar(char c) {
    while ((UART[UART_LSR] & LSR_TX_EMPTY) == 0) ;
    UART[UART_THR] = c;
}

#endif
