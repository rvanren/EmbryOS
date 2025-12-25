#include "embryos.h"
#include "fdt_embryos.h"

// Table of all known UART types
extern void uart_ns16550_init(uintptr_t), uart_ns16550_putchar(uintptr_t, char c), uart_ns16550_isr(uintptr_t);
extern void uart_sifive_init(uintptr_t),  uart_sifive_putchar(uintptr_t, char c),  uart_sifive_isr(uintptr_t);
extern void uart_pxa_init(uintptr_t),     uart_pxa_putchar(uintptr_t, char c),     uart_pxa_isr(uintptr_t);
extern void uart_litex_init(uintptr_t),   uart_litex_putchar(uintptr_t, char c),   uart_litex_isr(uintptr_t);
static struct uart_info uart_info[] = {
    { "ns16550a",    uart_ns16550_init, uart_ns16550_putchar, uart_ns16550_isr },
    { "dw-apb-uart", uart_pxa_init,     uart_pxa_putchar,     uart_pxa_isr     },
    { "uart0",       uart_sifive_init,  uart_sifive_putchar,  uart_sifive_isr  },
    { "pxa-uart",    uart_pxa_init,     uart_pxa_putchar,     uart_pxa_isr     },
    { "litex",       uart_litex_init,   uart_litex_putchar,   uart_litex_isr   },
};

struct uart_info *uart = 0;      // Console ("debug") UART of this platform

static int match_uart_type(const char *compat, const struct uart_info *ui, int count) {
    for (int i = 0; i < count; i++)
        if (strstr(compat, ui[i].type)) return i;
    return -1;
}

void uart_init(const char *compat, int len, uintptr_t base) {
    if (len == 0) len = strlen(compat);
    const char *next = compat, *end = compat + len;
    while (next < end) {
        int idx = match_uart_type(next, uart_info, sizeof(uart_info) / sizeof(uart_info[0]));
        if (idx >= 0) {
            uart = &uart_info[idx];
            uart->base = base;
            uart->init(uart->base);
            L2(L_BASE, L_UART_INIT, idx, base);
            kprintf("UART %s (%s) at %X\n", next, uart->type, base);
            break;
        }
        next += strlen(next) + 1;
    }
    if (uart == 0) kprintf("No %s UART found\n", compat);
}
