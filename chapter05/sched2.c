#include "embryos.h"

extern struct pcb *run_queue[];
static struct pcb *zombies;

void reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        proc_release(pcb);
    }
}

void sched_idle() {
    struct pcb *self = sched_self();
    L1(L_BASE, L_SCHED_IDLE, self->hart->id);
    for (;;) {
        proc_enqueue(&run_queue[2], self);
        sched_block(self);
        trap_wfi(self->hart->interrupts_work);
        uart->isr(uart->base);      // see if there's keyboard input
    }
}

void sched_exit(void) {
    struct pcb *self = sched_self();
    L0(L_NORM, L_SCHED_EXIT);
    io_exit(self);
    self->next = zombies;
    zombies = self;
    sched_block(self);
}
