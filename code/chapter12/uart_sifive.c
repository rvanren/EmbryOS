#include <stdint.h>
#include "platform.h"
#include "uart.h"
#include "io.h"

#ifdef UART_SIFIVE

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };

#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void uart_init(void) {
    UART->rxctrl = 1;     // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_isr(void) {
    for (;;) {
        uint32_t val = UART->rxdata;
        if (val & FULL) break;
        io_received(val & 0xFF);
    }
}

#endif
