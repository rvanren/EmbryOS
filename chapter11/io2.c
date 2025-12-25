#include "embryos.h"

// io_focus: the process that have the input focus, if any
// io_wait:  circular queue of processes that do not have the focus but want input
struct pcb *io_wait, *io_focus;

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

static void io_tab(void) {  // somebody typed TAB: switch focus
    if (io_wait == 0) { // no one else waiting
        if (io_focus == 0) io_putchar(7 /* beep */);  // no one can get focus
        return;
    }
    if (io_focus != 0) io_add(io_focus, USER_GET_LOST_FOCUS);
    io_focus = proc_dequeue(&io_wait);   // assign new focus
    io_add(io_focus, USER_GET_GOT_FOCUS);
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
    if (self == io_focus) io_focus = 0;
}
