#include "uart.h"
#include "sched.h"
#include "platform.h"

extern struct pcb *uart_focus, *uart_wait;
extern void uart_tab(void), uart_char(char c);

void uart_received(char c) {
    if (c == '\t') uart_tab();
    else if (uart_focus != 0) uart_char(c);
    else uart_putchar(7);    // beep
}

int uart_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu) {
    while (self->kbd_size == 0) {
        if (!self->kbd_warm) {
            if (uart_focus != 0 && uart_focus->kbd_waiting)
                uart_put(uart_focus, uart_focus->cu);
            uart_focus = self;
            self->kbd_warm = 1;
        }
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
