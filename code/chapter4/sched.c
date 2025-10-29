#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"
#include "interrupt.h"

struct pcb *run_queue;

void sched_init(struct pcb *first) {
    proc_enqueue(&run_queue, first);
}

void sched_block(struct pcb *current) {
    struct pcb *next = run_queue->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
    proc_reap_zombies();
}

void sched_yield(void) {
    struct pcb *current = run_queue->next;
    run_queue = current;
    sched_block(current);
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *current = run_queue->next;
    struct pcb *pcb = proc_create(executable, area, args, size);
    proc_enqueue(&run_queue, pcb);
    ctx_start(&current->sp, (char *) pcb + PAGE_SIZE);
    proc_reap_zombies();
}

void sched_idle() {
    for (;;) sched_yield();
}
