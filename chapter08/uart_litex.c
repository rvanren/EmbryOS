#include "embryos.h"

#define REG(base, index) (*(((volatile uint32_t *) (base)) + (index)))

#define UART_TXDATA    0
#define UART_TXFULL    1
#define UART_RXDATA    0
#define UART_RXEMPTY   2
#define UART_EVPEND    4

void uart_litex_init(uintptr_t base) { }

void uart_litex_putchar(uintptr_t base, char c) {
    while (REG(base, UART_TXFULL)) ;
    REG(base, UART_TXDATA) = c & 0xFF;
}

void uart_litex_isr(uintptr_t base) {
    while (!REG(base, UART_RXEMPTY)) {
        io_received(REG(base, UART_RXDATA) & 0xFF);
        REG(base, UART_EVPEND) = 0x2;
    }
}
