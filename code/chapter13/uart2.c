#include "uart.h"
#include "sched.h"
#include "platform.h"

extern struct pcb *uart_focus;
extern struct pcb *uart_wait;
extern void uart_tab(void);
extern void uart_char(char c);

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };
#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}

void uart_init(void) {
    UART->rxctrl = 1;     // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_isr(void) {
    for (;;) {
        uint32_t val = UART->rxdata;
        if (val & FULL) break;
        if ((val & 0xFF) == '\t') uart_tab();
        else if (uart_focus != 0) uart_char(val & 0xFF);
    }
}

int uart_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu) {
    while (self->kbd_size == 0) {
        if (uart_focus == 0) uart_focus = self;
        proc_put(self, row, col, self == uart_focus ? cf : cu);
        self->cf = cf; self->cu = cu;
        self->kbd_waiting = 1;
        self->kbd_row = row; self->kbd_col = col;
        proc_dequeue(&run_queue[proc_current]);
        if (uart_wait == 0) uart_wait = self->next = self;
        else { self->next = uart_wait->next; uart_wait->next = self; }
        sched_block(self);
    }
    char c = self->kbd_buf[self->kbd_tail];
    self->kbd_tail = (self->kbd_tail + 1) % KBD_BUF_SIZE;
    self->kbd_size--;
    return c;
}

void uart_exit(struct pcb *self) {
    if (self == uart_focus) uart_focus = 0;
}
