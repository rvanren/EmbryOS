#include <stdint.h>
#include "uart.h"
#include "sched.h"
#include "platform.h"

struct pcb *uart_wait = 0;    // circular wait queue
struct pcb *uart_focus;       // keyboard focus

static void uart_put(struct pcb *pcb, cell_t c) {
    proc_put(pcb, pcb->kbd_row, pcb->kbd_col, c);
}

void uart_tab(void) {
    if (uart_wait == 0) return;
    if (uart_focus == 0) {
        uart_focus = uart_wait;
        uart_put(uart_focus, uart_focus->cf);
    } else {
        uart_put(uart_focus, uart_focus->cu);
        if (uart_focus == uart_wait) uart_wait = uart_wait->next;
        uart_focus = uart_wait;
        uart_put(uart_focus, uart_focus->cf);
    }
}

void uart_char(char c) {
    struct pcb *pcb = uart_focus;
    if (pcb->kbd_size >= KBD_BUF_SIZE) { putchar(7 /* beep */); return; }
    if (pcb->kbd_waiting) {
        pcb->kbd_waiting = 0;
        if (pcb == pcb->next) uart_wait = 0;
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
