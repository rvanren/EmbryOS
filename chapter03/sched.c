#include "embryos.h"

static struct pcb *run_queue, *zombies;

void sched_resume(struct pcb *pcb) { proc_enqueue(&run_queue, pcb); }

void reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        proc_release(pcb);
    }
}

void sched_block(struct pcb *old) {
    struct pcb *new = proc_dequeue(&run_queue);
    if (new != old) {
        new->hart = old->hart;
        sched_set_self(new);
        L3(L_FREQ, L_CTX_SWITCH, (uintptr_t) old, (uintptr_t) new, new->hart->id);
        ctx_switch(&old->sp, new->sp);
        reap_zombies();
    }
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *old = sched_self();
    proc_enqueue(&run_queue, old);
    struct pcb *new = proc_create(old->hart, executable, area, args, size);
    sched_set_self(new);
    L4(L_NORM, L_CTX_START, (uintptr_t) old, (uintptr_t) new, new->hart->id, executable);
    ctx_start(&old->sp, (char *) new + PAGE_SIZE);
    reap_zombies();
}

void sched_exit(void) {
    struct pcb *self = sched_self();
    L0(L_NORM, L_SCHED_EXIT);
    self->next = zombies;
    zombies = self;
    sched_block(self);
}

void sched_yield(void) {
    struct pcb *self = sched_self();
    proc_enqueue(&run_queue, self);
    sched_block(self);
}

void sched_idle() {
    L1(L_BASE, L_SCHED_IDLE, sched_self()->hart->id);
    for (;;) sched_yield();
}
