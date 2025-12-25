#include "embryos.h"

void io_putchar(char c) {
    L1(L_FREQ, L_IO_PUTCHAR, c & 0xFF);
    extern struct uart_info *uart;
    if (uart == 0) sbi_putchar(c);
    else uart->putchar(uart->base, c);
}

// Process 'self' wants input
int io_get(struct pcb *self, int block) {
    extern struct pcb *io_focus, *io_wait;

    L3(block ? L_NORM : L_FREQ, L_IO_GET, block, self->kbd_size, (uintptr_t) io_focus);
    while (self->kbd_size == 0) {       // block until there's input
        if (!self->kbd_warm) {  // first time: always gets the focus
            if (io_focus != 0) io_add(io_focus, USER_GET_LOST_FOCUS);
            io_focus = self;
            self->kbd_warm = 1;
        }
        if (!block) return USER_GET_NO_INPUT;
        self->kbd_waiting = 1;
        if (self != io_focus) proc_enqueue(&io_wait, self);
        sched_block(self);
    }

    // Take character from the circular buffer.
    int c = self->kbd_buf[self->kbd_head];
    self->kbd_head = (self->kbd_head + 1) % KBD_BUF_SIZE;
    self->kbd_size--;
    return c;
}
