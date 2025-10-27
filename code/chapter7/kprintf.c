#include <stdarg.h>
#include "kprintf.h"
#include "uart.h"

static void print_unsigned(unsigned int x, unsigned int base) {
    char buf[16];
    int i = 0;
    do {
        int digit = x % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + (digit - 10);
        x /= base;
    } while (x != 0);
    while (--i >= 0) uart_putchar(buf[i]);
}

void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { uart_putchar(*fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': { int x = va_arg(ap, int);
            if (x < 0) { uart_putchar('-'); x = -x; }
            print_unsigned((unsigned int) x, 10);
            break;
        }
        case 'u': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(x, 10);
            break;
        }
        case 'x': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(x, 16);
            break;
        }
        case 's': { char *s = va_arg(ap, char *);
            while (*s) uart_putchar(*s++);
            break;
        }
        case 'c': { int c = va_arg(ap, int);
            uart_putchar(c);
            break;
        }
        case '%':
            uart_putchar('%');
            break;
        default:
            uart_putchar('%'); uart_putchar(*fmt);
        }
    }
    va_end(ap);
}
