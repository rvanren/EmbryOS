#include "uart.h"
#include "sched.h"
#include "platform.h"

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };

#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void uart_init(void) {
    UART->rxctrl = 1;     // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}
