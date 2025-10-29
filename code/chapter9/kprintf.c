#include <stdarg.h>
#include "kprintf.h"
#include "io.h"

static void print_unsigned(unsigned int x, unsigned int base) {
    char buf[16];
    int i = 0;
    do {
        int digit = x % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + (digit - 10);
        x /= base;
    } while (x != 0);
    while (--i >= 0) io_putchar(buf[i]);
}

void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { io_putchar(*fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': { int x = va_arg(ap, int);
            if (x < 0) { io_putchar('-'); x = -x; }
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
            while (*s) io_putchar(*s++);
            break;
        }
        case 'c': { int c = va_arg(ap, int);
            io_putchar(c);
            break;
        }
        case '%':
            io_putchar('%');
            break;
        default:
            io_putchar('%'); io_putchar(*fmt);
        }
    }
    va_end(ap);
}
