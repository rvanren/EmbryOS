#include "uart.h"
#include "sched.h"
#include "io.h"

extern struct pcb *io_focus, *io_wait;

void io_putchar(char c) { uart_putchar(c); }

void io_put(struct pcb *pcb, cell_t c) {
    proc_put(pcb, pcb->kbd_row, pcb->kbd_col, c);
}

int io_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu) {
    while (self->kbd_size == 0) {
        if (!self->kbd_warm) {
            if (io_focus != 0 && io_focus->kbd_waiting)
                io_put(io_focus, io_focus->cu);
            io_focus = self;
            self->kbd_warm = 1;
        }
        proc_put(self, row, col, self == io_focus ? cf : cu);
        self->cf = cf; self->cu = cu;
        self->kbd_waiting = 1;
        self->kbd_row = row; self->kbd_col = col;
        proc_dequeue(&run_queue[proc_current]);
        if (io_wait == 0) io_wait = self->next = self;
        else { self->next = io_wait->next; io_wait->next = self; }
        sched_block(self);
    }
    char c = self->kbd_buf[self->kbd_tail];
    self->kbd_tail = (self->kbd_tail + 1) % KBD_BUF_SIZE;
    self->kbd_size--;
    return c;
}
