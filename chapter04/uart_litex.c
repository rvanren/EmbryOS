#include "embryos.h"

struct uart { uint32_t data, txfull, status, unknown, evpend; };

#define UART(base)  ((volatile struct uart *) (base))

void uart_litex_init(uintptr_t base) { }

void uart_litex_putchar(uintptr_t base, char c) {
    // while (UART(base)->status & 0x1) ;
    while (UART(base)->txfull) ;
    UART(base)->data = (uint32_t) c;
}
