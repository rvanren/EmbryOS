#include "uart.h"
#include "io.h"

void io_putchar(char c) {
    uart_putchar(c);
}
