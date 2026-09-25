#include "embryos.h"

static struct pcb *io_focus;   // the process that has the input focus, if any

// Process 'self' wants input
int io_get(struct pcb *self, int block) {
    L3(block ? L_NORM : L_FREQ, L_IO_GET, block, self->kbd_size, (uintptr_t) io_focus);
    while (self->kbd_size == 0) {       // block until there's input
        if (!self->kbd_warm) {  // first time: always gets the focus
            if (io_focus != 0) {
                io_add(io_focus, USER_GET_LOST_FOCUS);
                io_focus->last_focus = mtime_get();
            }
            io_focus = self;
            self->kbd_warm = 1;
            self->last_focus = mtime_get() + 1;
        }
        if (!block) return USER_GET_NO_INPUT;
        self->kbd_waiting = 1;
        sched_block(self, KBD_WAIT);
    }

    // Take character from the circular buffer.
    int c = self->kbd_buf[self->kbd_head];
    self->kbd_head = (self->kbd_head + 1) % KBD_BUF_SIZE;
    self->kbd_size--;
    return c;
}

void io_add(struct pcb *pcb, int c) {
    L4(L_NORM, L_IO_ADD, (uintptr_t) pcb, c, pcb->kbd_size, pcb->kbd_waiting);
    if (pcb->kbd_size == KBD_BUF_SIZE)
        pcb->kbd_buf[(pcb->kbd_head + KBD_BUF_SIZE - 1) % KBD_BUF_SIZE] = c;
    else {
        pcb->kbd_buf[(pcb->kbd_head + pcb->kbd_size) % KBD_BUF_SIZE] = c;
        pcb->kbd_size++;
    }
    if (pcb->kbd_waiting) {     // wake up process
        pcb->kbd_waiting = 0;
        sched_resume(pcb);
    }
}

static int io_cmp(struct pcb *best, struct pcb *pcb) {
    if (!pcb->kbd_warm) return -1;
    if (best == 0) return 1;
    return best->last_focus < pcb->last_focus ? -1 : 1;
}

static void io_tab(void) {  // somebody typed TAB: switch focus
    struct pcb *pcb = sched_find(io_cmp);
    if (pcb == 0) {
        io_putchar(7 /* beep */);  // no one can get focus
        return;
    }
    if (io_focus != 0) {
        io_add(io_focus, USER_GET_LOST_FOCUS);
        io_focus->last_focus = mtime_get();
    }
    io_focus = pcb;
    io_focus->last_focus = mtime_get() + 1;
    io_add(pcb, USER_GET_GOT_FOCUS);
    return;
}

static void io_char(char c) {   // somebody typed c
    struct pcb *pcb = io_focus;
    if (pcb->kbd_size == KBD_BUF_SIZE) { io_putchar(7 /* beep */); return; }
    io_add(pcb, c & 0xFF);
}

void io_received(char c) {
    L2(L_NORM, L_IO_RECEIVED, (uintptr_t) io_focus, c & 0xFF);
    if (c == '\t') io_tab();
    else if (io_focus != 0) io_char(c);
    else io_putchar(7);    // beep: no process has the focus
}

void io_exit(struct pcb *self) {
    if (!self->kbd_warm) return;
    if (self == io_focus) io_focus = 0;
}
