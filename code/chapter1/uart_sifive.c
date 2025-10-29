#include "platform.h"
#include "io.h"
#include "uart.h"

#ifdef UART_SIFIVE

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };

#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void uart_init(void) {
}

void uart_putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}

#endif
