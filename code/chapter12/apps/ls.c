#include <stdarg.h>
#include "syslib.h"
#include "screen.h"
#include "string.h"

static void print_unsigned(struct screen *screen, unsigned int x, unsigned int base) {
    char buf[16];
    int i = 0;
    do {
        int digit = x % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + (digit - 10);
        x /= base;
    } while (x != 0);
    while (--i >= 0) screen_putchar(screen, buf[i]);
}

void printf(struct screen *screen, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { screen_putchar(screen, *fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': { int x = va_arg(ap, int);
            if (x < 0) { screen_putchar(screen, '-'); x = -x; }
            print_unsigned(screen, (unsigned int) x, 10);
            break;
        }
        case 'u': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(screen, x, 10);
            break;
        }
        case 'x': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(screen, x, 16);
            break;
        }
        case 's': { char *s = va_arg(ap, char *);
            while (*s) screen_putchar(screen, *s++);
            break;
        }
        case 'c': { int c = va_arg(ap, int);
            screen_putchar(screen, c);
            break;
        }
        case '%':
            screen_putchar(screen, '%');
            break;
        default:
            screen_putchar(screen, '%'); putchar(*fmt);
        }
    }
    va_end(ap);
}

void main(void) {
    struct screen screen;
    char line[128];

    screen_init(&screen);
    screen_sync(&screen);

    printf("SIZE %d\n", user_size(1));
}
