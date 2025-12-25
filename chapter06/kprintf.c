#include <stdarg.h>
#include "embryos.h"

static void print_unsigned(uword_t x, unsigned int base) {
    char buf[32];
    int i = 0;
    do {
        int d = x % base;
        buf[i++] = (d < 10) ? '0'+d : 'a'+(d-10);
        x /= base;
    } while (x);
    while (--i >= 0) io_putchar(buf[i]);
}

void kprintf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { io_putchar(*fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': {
            int x = va_arg(ap, int);
            if (x < 0) { io_putchar('-'); x = -x; }
            print_unsigned((uword_t) x, 10);
            break;
        }
        case 'D': {
            sword_t x = va_arg(ap, sword_t);
            if (x < 0) { io_putchar('-'); x = -x; }
            print_unsigned((uword_t) x, 10);
            break;
        }
        case 'u': print_unsigned(va_arg(ap, unsigned int), 10); break;
        case 'U': print_unsigned(va_arg(ap, uword_t),      10); break;
        case 'x': print_unsigned(va_arg(ap, unsigned int), 16); break;
        case 'X': print_unsigned(va_arg(ap, uword_t),      16); break;
        case 'p': print_unsigned(va_arg(ap, uintptr_t),    16); break;
        case 'c': io_putchar(va_arg(ap, int)); break;
        case 's': {
            char *s = va_arg(ap, char*);
            while (*s) io_putchar(*s++);
            break;
        }
        case '%': io_putchar('%'); break;
        default:  io_putchar('%'); io_putchar(*fmt);
        }
    }
    va_end(ap);
}
