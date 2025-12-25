#include "embryos.h"

// Table of all known UART types
extern void uart_ns16550_init(uintptr_t),  uart_ns16550_putchar(uintptr_t, char);
extern void uart_sifive_init(uintptr_t),   uart_sifive_putchar(uintptr_t, char);
extern void uart_pxa_init(uintptr_t),      uart_pxa_putchar(uintptr_t, char);
extern void uart_litex_init(uintptr_t),    uart_litex_putchar(uintptr_t, char);
static struct uart_info uart_info[] = {
    { "ns16550a",    uart_ns16550_init,  uart_ns16550_putchar  },
    { "dw-apb-uart", uart_pxa_init,      uart_pxa_putchar },
    { "uart0",       uart_sifive_init,   uart_sifive_putchar   },
    { "pxa-uart",    uart_pxa_init,      uart_pxa_putchar      },
    { "litex",       uart_litex_init,    uart_litex_putchar    },
};

struct uart_info *uart = 0;      // Console ("debug") UART of this platform

static int match_uart_type(const char *compat, const struct uart_info *ui, int count) {
    for (int i = 0; i < count; i++)
        if (strstr(compat, ui[i].type)) return i;
    return -1;
}

void uart_init(const char *compat, int len, uintptr_t base) {
    if (len == 0) len = strlen(compat);
    const char *end = compat + len;
    while (compat < end) {
        int idx = match_uart_type(compat, uart_info, sizeof(uart_info) / sizeof(uart_info[0]));
        if (idx >= 0) {
            uart = &uart_info[idx];
            uart->base = base;
            uart->init(uart->base);
            L2(L_BASE, L_UART_INIT, idx, base);
            kprintf("UART %s at %X\n", compat, base);
            break;
        }
        compat += strlen(compat) + 1;
    }
}
