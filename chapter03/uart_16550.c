#include "embryos.h"

#define UART_THR    0x00  // Transmit Holding Register (write)
#define UART_RBR    0x00  // Receiver Buffer Register (read)
#define UART_IER    0x01  // Interrupt Enable Register
#define UART_LSR    0x05  // Line Status Register

#define LSR_DATA_READY 0x01
#define LSR_THR_EMPTY  0x20  // Transmitter Holding Register Empty

#define UART(base) ((volatile uint8_t *) (base))

void uart_ns16550_init(uintptr_t base) { }

void uart_ns16550_putchar(uintptr_t base, uint8_t ch) {
    while ((UART(base)[UART_LSR] & LSR_THR_EMPTY) == 0) ;
    UART(base)[UART_THR] = ch;
}
