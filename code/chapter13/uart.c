#include <stdint.h>
#include "uart.h"
#include "sched.h"
#include "platform.h"
#include "screen.h"

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };
#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

static struct pcb *uart_wait = 0;    // circular wait queue
static struct pcb *uart_focus;       // keyboard focus

void putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}

void uart_init(void) {
    UART->rxctrl = 1;     // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_tab(void) {
    if (uart_wait == 0) return;
    uart_wait = uart_wait->next;
    if (uart_focus == 0) {
        uart_focus = uart_wait;
        screen_move(uart_focus->kbd_row, uart_focus->kbd_col);
        screen_put(uart_focus->cf);
    }
    else {
        screen_move(uart_focus->kbd_row, uart_focus->kbd_col);
        screen_put(uart_focus->cu);
        uart_focus = uart_wait;
        screen_move(uart_focus->kbd_row, uart_focus->kbd_col);
        screen_put(uart_focus->cf);
    }
}

static void uart_char(char c) {
    // assert uart_focus != 0
    struct pcb *pcb = uart_focus;

    if (pcb->kbd_size < KBD_BUF_SIZE) {
        if (pcb->kbd_waiting) {
            // assert pcb->kbd_size == 0
            pcb->kbd_waiting = 0;
            if (pcb == pcb->next) {
                // assert uart_wait == pcb
                uart_wait = 0;
            }
            else {
                struct pcb *prev = uart_wait;
                while (prev->next != pcb) prev = prev->next;
                prev->next = pcb->next;
                if (uart_wait == pcb) uart_wait = prev;
            }
            proc_enqueue(&run_queue[0], pcb);
        }
        pcb->kbd_buf[(pcb->kbd_tail + pcb->kbd_size) % KBD_BUF_SIZE] = c;
        pcb->kbd_size++;
    }
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
        screen_move(row, col);
        screen_put(self == uart_focus ? cf : cu);
        self->cf = cf; self->cu = cu;
        self->kbd_waiting = 1;
        self->kbd_row = row; self->kbd_col = col;
        proc_dequeue(&run_queue[proc_current]);
        // assert return value == self
        if (uart_wait == 0) uart_wait = self->next = self;
        else { self->next = uart_wait->next; uart_wait->next = self; }
        sched_block(self);
    }
    // assert !self->kbd_waiting
    char c = self->kbd_buf[self->kbd_tail];
    self->kbd_tail = (self->kbd_tail + 1) % KBD_BUF_SIZE;
    self->kbd_size--;
    return c;
}

void uart_exit(struct pcb *self) {
    // assert !self->kbd_waiting
    if (self == uart_focus) uart_focus = 0;
}
