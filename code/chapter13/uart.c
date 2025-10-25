#include <stdint.h>
#include "uart.h"
#include "sched.h"
#include "platform.h"
#include "screen.h"

#define KBD_BUF_SIZE 64

struct uart { uint32_t txdata, rxdata, txctrl, rxctrl, ie, ip; };
#define UART ((volatile struct uart *) UART_BASE)
#define FULL (1 << 31)

static char buf[KBD_BUF_SIZE];		 // receive buffer
static int head = 0, tail = 0;
static struct pcb *uart_wait = 0;        // wait queue

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
        int next = (head + 1) % KBD_BUF_SIZE;
        if (next != tail) {
            buf[head] = val & 0xFF;
            head = next;
        }
    }
    while (uart_wait != 0) { // Put all sleepers on the run queue
        struct pcb *next = uart_wait->next;
        proc_enqueue(&run_queue[0], uart_wait);
        uart_wait = next;
    }
}

int uart_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu) {
    screen_move(row, col);
    screen_put(cf);
    while (head == tail) {
        struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
        pcb->next = uart_wait;
        uart_wait = pcb;
        sched_block(pcb);
    }
    char c = buf[tail];
    tail = (tail + 1) % KBD_BUF_SIZE;
    return c;
}
