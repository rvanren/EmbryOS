#include "embryos.h"

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl; };

#define UART(base)  ((volatile struct uart *) (base))

#define EMPTY       (1u << 31)

void uart_sifive_init(uintptr_t base) { }

void uart_sifive_putchar(uintptr_t base, char c) {
    while (UART(base)->txdata & (1u << 31)) ;
    UART(base)->txdata = (uint32_t) c;
}
