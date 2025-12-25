#include "embryos.h"

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl; };

#define UART(base)  ((volatile struct uart *) (base))

#define TXFULL     (1u << 31)   // TXDATA[31]=1 means FIFO full
#define EMPTY      (1u << 31)   // RXDATA[31]=1 means FIFO empty

void uart_sifive_init(uintptr_t base) {
    UART(base)->rxctrl = 1;      // enable receiver (bit 0)
}

void uart_sifive_putchar(uintptr_t base, char c) {
    while (UART(base)->txdata & TXFULL) ;
    UART(base)->txdata = (uint32_t)(uint8_t)c;
}

void uart_sifive_isr(uintptr_t base) {
    for (;;) {
        uint32_t val = UART(base)->rxdata;
        if (val & EMPTY) break;     // stop if FIFO empty
        io_received(val & 0xFF);    // deliver received byte
    }
}
