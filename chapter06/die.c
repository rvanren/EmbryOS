#include "embryos.h"

void die(void *msg) {
    struct pcb *self = sched_self();
    L0(L_NORM, L_DIE);
    if (self != 0 && self->executable > 0)
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("%s", msg);
    if (self != 0 && self->executable > 0) {
        kprintf("<");
        sched_exit();
    }
    else {
        kprintf("\n");
        for (;;) ;
    }
}
