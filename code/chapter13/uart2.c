#include "uart.h"
#include "sched.h"
#include "platform.h"

extern struct pcb *uart_focus, *uart_wait;
extern void uart_tab(void), uart_char(char c);

#ifdef SIFIVE
struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };

#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

void uart_init(void) {
    UART->rxctrl = 1;     // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_putchar(char c) {
    while (UART->txdata & FULL) ;
    UART->txdata = c;
}

void uart_isr(void) {
    for (;;) {
        uint32_t val = UART->rxdata;
        if (val & FULL) break;
        if ((val & 0xFF) == '\t') uart_tab();
        else if (uart_focus != 0) uart_char(val & 0xFF);
        else uart_putchar(7);    // beep
    }
}
#endif // SIFIVE

#ifdef VIRT
#include <stdint.h>

// 16550A register offsets (byte addressed)
#define UART_RBR    0x00  // Receiver Buffer Register (read)
#define UART_THR    0x00  // Transmit Holding Register (write)
#define UART_LSR    0x05  // Line Status Register
#define UART_IER    0x01  // Interrupt Enable Register

#define LSR_DATA_READY 0x01
#define LSR_TX_EMPTY   0x20

static volatile uint8_t *UART = (volatile uint8_t *) UART_BASE;

void uart_init(void) {
    UART[UART_IER] = 0x01;  // enable received data interrupt
}

void uart_putchar(char c) {
    while ((UART[UART_LSR] & LSR_TX_EMPTY) == 0) ;
    UART[UART_THR] = c;
}

void uart_isr(void) {
    for (;;) {
        if ((UART[UART_LSR] & LSR_DATA_READY) == 0)
            break;
        char c = UART[UART_RBR];
        if (c == '\t') uart_tab();
        else if (uart_focus != 0) uart_char(c);
        else uart_putchar(7);   // beep
    }
}

#endif // VIRT

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
