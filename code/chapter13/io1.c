#include "sched.h"
#include "io.h"

struct pcb *io_wait = 0;    // circular wait queue
struct pcb *io_focus;       // keyboard focus

void io_put(struct pcb *pcb, cell_t c) {
    proc_put(pcb, pcb->kbd_row, pcb->kbd_col, c);
}

void io_tab(void) {
    if (io_wait == 0) { if (io_focus == 0) io_putchar(7); }
    else if (io_focus == 0) {
        io_focus = io_wait;
        io_put(io_focus, io_focus->cf);
    }
    else {
        io_put(io_focus, io_focus->cu);
        if (io_focus == io_wait) io_wait = io_wait->next;
        io_focus = io_wait;
        io_put(io_focus, io_focus->cf);
    }
}

void io_char(char c) {
    struct pcb *pcb = io_focus;
    if (pcb->kbd_size >= KBD_BUF_SIZE) { io_putchar(7 /* beep */); return; }
    if (pcb->kbd_waiting) {
        pcb->kbd_waiting = 0;
        if (pcb == pcb->next) io_wait = 0;
        else {
            struct pcb *prev = io_wait;
            while (prev->next != pcb) prev = prev->next;
            prev->next = pcb->next;
            if (io_wait == pcb) io_wait = prev;
        }
        proc_enqueue(&run_queue[0], pcb);
    }
    pcb->kbd_buf[(pcb->kbd_tail + pcb->kbd_size) % KBD_BUF_SIZE] = c;
    pcb->kbd_size++;
}

void io_exit(struct pcb *self) {
    if (self == io_focus) io_focus = 0;
}
