#include "kbd.h"
#include "waitq.h"
#include "interrupt.h"

#define KBD_BUF_SIZE 64
#define UART_RX (volatile uint8_t *) 0x10000000

static char buf[KBD_BUF_SIZE];
static int head = 0, tail = 0;
static struct pcb *kbd_wait = 0;        // wait queue

void kbd_init(void) {
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
        kbd_wait = kdb_wait->next;
    }
}

int kbd_get(void) {
    // sleep while no characters
    while (head == tail) {
        struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
        pcb->next = kbd_wait;
        kbd_wait = pcb;
        sched_block();
    }

    char c = buf[tail];
    tail = (tail + 1) % KBD_BUF_SIZE;
    return c;
}
