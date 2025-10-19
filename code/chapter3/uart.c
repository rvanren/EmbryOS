#include <stdint.h>
#include "uart.h"
#include "platform.h"

#define KBD_BUF_SIZE 64

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };
#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}

void uart_init(void) {
}
