#include "embryos.h"

void io_putchar(char c) {
    L1(L_FREQ, L_IO_PUTCHAR, c & 0xFF);
    extern struct uart_info *uart;
    if (uart == 0) sbi_putchar(c);
    else uart->putchar(uart->base, c);
}
