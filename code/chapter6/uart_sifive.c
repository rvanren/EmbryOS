#include <stdint.h>
#include "platform.h"
#include "uart.h"
#include "io.h"

#ifdef UART_SIFIVE

#include <stdint.h>
#include "platform.h"

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };

#define UART       ((volatile struct uart *)(uintptr_t)UART_BASE)
#define EMPTY      (1u << 31)   // RXDATA[31]=1 means FIFO empty

void uart_init(void) {
    UART->rxctrl = 1;      // enable receiver (bit 0)
    UART->ie     = (1u << 1);  // enable RX interrupt (bit 1)
}

void uart_isr(void) {
    for (;;) {
        uint32_t val = UART->rxdata;
        if (val & EMPTY) break;     // stop if FIFO empty
        io_received(val & 0xFF);    // deliver received byte
    }
}

#endif
