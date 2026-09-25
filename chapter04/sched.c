#include "embryos.h"

static struct pcb *proc_table;

struct pcb *sched_find(int (*cmp)(struct pcb *, struct pcb *)) {
    struct pcb *best = 0;

    for (struct pcb *pcb = proc_table; pcb != 0; pcb = pcb->next)
        if (pcb->state != ZOMBIE)
            if ((*cmp)(best, pcb) >= 0) best = pcb;
    return best;
}

static void reap_zombies() {
    struct pcb *self = sched_self(), *pcb;

    for (struct pcb **pp = &proc_table; (pcb = *pp) != 0;)
        if (pcb->state == ZOMBIE && pcb != self) {
            *pp = pcb->next;
            proc_release(pcb);
        }
        else pp = &pcb->next;
}

static int best_runnable(struct pcb *best, struct pcb *next) {
    if (next->state != RUNNABLE) return -1;
    if (best == 0) return 1;
    return best->wait_start < next->wait_start ? -1 : 1;
}

void sched_block(struct pcb *old, enum proc_state state) {
    old->state = state;
    struct pcb *pcb = sched_find(best_runnable);
    pcb->state = RUNNING;
    if (pcb != old) {
        pcb->hart = old->hart;
        sched_set_self(pcb);
        L3(L_FREQ, L_CTX_SWITCH, (uintptr_t) old, (uintptr_t) pcb, pcb->hart->id);
        ctx_switch(&old->sp, pcb->sp);
        reap_zombies();
    }
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *old = sched_self();
    old->state = RUNNABLE;
    old->wait_start = mtime_get();
    struct pcb *new = proc_create(old->hart, executable, area, args, size);
    sched_init(new);
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
    for (;;) {
        self->wait_start = (uint64_t) -1;      // infinity
        sched_block(self, RUNNABLE);
        trap_wfi(self->hart->interrupts_work);
    }
}

void sched_init(struct pcb *pcb) {
    sched_set_self(pcb);
    pcb->state = RUNNING;
    pcb->next = proc_table;
    proc_table = pcb;
}
