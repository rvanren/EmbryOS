#include <stdint.h>
#include "kbd.h"
#include "interrupt.h"
#include "sched.h"

#define KBD_BUF_SIZE 64
#define UART_RX (volatile unsigned char *) 0x10000000

#define UART_BASE   0x10000000
#define UART_TXDATA (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_RXDATA (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_TXCTRL (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_RXCTRL (*(volatile uint32_t *)(UART_BASE + 0x0C))
#define UART_IE     (*(volatile uint32_t *)(UART_BASE + 0x10))
#define UART_IP     (*(volatile uint32_t *)(UART_BASE + 0x14))

static char buf[KBD_BUF_SIZE];
static int head = 0, tail = 0;
static struct pcb *kbd_wait = 0;        // wait queue

void kbd_init(void) {
    UART_RXCTRL = 1;  // enable receiver (bit 0)
    UART_IE = (1 << 1);  // enable RX interrupt (bit 1)
}

void kbd_isr(void) {
    char c = *UART_RX;
    int next = (head + 1) % KBD_BUF_SIZE;
    if (next != tail) {
        buf[head] = c;
        head = next;
    }

    // Put all sleepers on the run queue
    while (kbd_wait != 0) {
        proc_enqueue(&run_queue[0], kbd_wait);
        kbd_wait = kbd_wait->next;
    }
}

int kbd_get(void) {
    // sleep while no characters
    while (head == tail) {
        struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
        pcb->next = kbd_wait;
        kbd_wait = pcb;
        sched_block(pcb);
    }

    char c = buf[tail];
    tail = (tail + 1) % KBD_BUF_SIZE;
    return c;
}
