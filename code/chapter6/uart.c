#include <stdint.h>
#include "kbd.h"
#include "interrupt.h"
#include "sched.h"
#include "stdio.h"

#define KBD_BUF_SIZE 64

struct uart {
    uint32_t txdata, rxdata;
    uint32_t txctrl, rxctrl;
    uint32_t ie, ip;
};

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define RXFULL (1 << 31)
#define TXFULL (1 << 31)

// Receive buffer
static char buf[KBD_BUF_SIZE];
static int head = 0, tail = 0;
static struct pcb *uart_wait = 0;        // wait queue

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}

void uart_init(void) {
    UART->rxctrl = 1;  // enable receiver (bit 0)
    UART->ie = (1 << 1);  // enable RX interrupt (bit 1)
}

void uart_isr(void) {
    for (;;) {
        uint32_t val = UART->rxdata;
        if (val & RXFULL) break;
        int next = (head + 1) % KBD_BUF_SIZE;
        if (next != tail) {
            buf[head] = val & 0xFF;
            head = next;
        }
    }

    // Put all sleepers on the run queue
    while (uart_wait != 0) {
        struct pcb *next = uart_wait->next;
        proc_enqueue(&run_queue[0], uart_wait);
        uart_wait = next;
    }
}

int uart_get(void) {
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
