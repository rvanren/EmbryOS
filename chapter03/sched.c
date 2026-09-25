#include "embryos.h"

static struct pcb *proc_table;

void sched_block(struct pcb *old, enum proc_state state) {
    struct pcb *new = 0, *pcb;

    old->state = state;
    for (struct pcb **pp = &proc_table; (pcb = *pp) != 0;)
        if (pcb->state == ZOMBIE && pcb != old) {
            *pp = pcb->next;
            proc_release(pcb);
        }
        else {
            if (pcb->state == RUNNABLE && (new == 0 || pcb->wait_start < new->wait_start))
                new = pcb;
            pp = &pcb->next;
        }

    new->state = RUNNING;
    if (new != old) {
        new->hart = old->hart;
        sched_set_self(new);
        L3(L_FREQ, L_CTX_SWITCH, (uintptr_t) old, (uintptr_t) new, new->hart->id);
        ctx_switch(&old->sp, new->sp);
    }
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *old = sched_self();
    old->state = RUNNABLE;
    old->wait_start = mtime_get();
    struct pcb *new = proc_create(old->hart, executable, area, args, size);
    new->next = proc_table;
    proc_table = new;
    new->state = RUNNING;
    sched_set_self(new);
    L4(L_NORM, L_CTX_START, (uintptr_t) old, (uintptr_t) new, new->hart->id, executable);
    ctx_start(&old->sp, (char *) new + PAGE_SIZE);
}

void sched_exit(void) {
    struct pcb *self = sched_self();
    L0(L_NORM, L_SCHED_EXIT);
    sched_block(self, ZOMBIE);
}

void sched_yield(void) {
    struct pcb *self = sched_self();
    self->wait_start = mtime_get();
    sched_block(self, RUNNABLE);
}

void sched_idle() {
    struct pcb *self = sched_self();
    L1(L_BASE, L_SCHED_IDLE, self->hart->id);
    for (;;) sched_yield();
}

void sched_init(struct pcb *pcb) {
    sched_set_self(pcb);
    pcb->next = proc_table;
    proc_table = pcb;
}
