#include "embryos.h"

#define UART_THR    0x00  // Transmit Holding Register (write)
#define UART_RBR    0x00  // Receiver Buffer Register (read)
#define UART_IER    0x01  // Interrupt Enable Register
#define UART_LSR    0x05  // Line Status Register

#define LSR_THRE       (1u << 5)   // THR empty
#define LSR_DATA_READY (1u << 0) 

#define UART(base) ((volatile uint8_t *) (base))

void uart_ns16550_init(uintptr_t base) { }

void uart_ns16550_putchar(uintptr_t base, char c) {
    while ((UART(base)[UART_LSR] & LSR_THRE) == 0) ;
    UART(base)[UART_THR] = (uint8_t) c;
}

void uart_ns16550_isr(uintptr_t base) {
    while (UART(base)[UART_LSR] & LSR_DATA_READY)
        io_received(UART(base)[UART_RBR] & 0xFF);
}
