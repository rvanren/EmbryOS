#include "stdio.h"

struct uart { uint32_t txdata; };

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define TXFULL (1 << 31)

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}
