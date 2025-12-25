#include "embryos.h"

#define N_PRIORITIES 3

struct pcb *run_queue[N_PRIORITIES];

void sched_resume(struct pcb *pcb) { proc_enqueue(&run_queue[0], pcb); }

void sched_block(struct pcb *old) {
    int p = 0;      // first find highest priority process
    while (p < N_PRIORITIES && run_queue[p] == 0) p++;
    struct pcb *new = proc_dequeue(&run_queue[p]);
    if (new != old) {       // switch needed
        new->hart = old->hart;
        new->hart->needs_tlb_flush = 1;
        sched_set_self(new);
        L3(L_FREQ, L_CTX_SWITCH, (uintptr_t) old, (uintptr_t) new, new->hart->id);
        ctx_switch(&old->sp, new->sp);
        reap_zombies();
    }
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *old = sched_self();
    proc_enqueue(&run_queue[0], old);
    struct pcb *new = proc_create(old->hart, executable, area, args, size);
    sched_set_self(new);
    L4(L_NORM, L_CTX_START, (uintptr_t) old, (uintptr_t) new, new->hart->id, executable);
    ctx_start(&old->sp, (char *) new + PAGE_SIZE);
    reap_zombies();
}

void sched_yield(void) {
    struct pcb *current = sched_self();
    if (current->executable > 0) {     // idle loop doesn't yield at interrupts
        proc_enqueue(&run_queue[1], current);
        sched_block(current);
    }
}
