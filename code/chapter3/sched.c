#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"

struct pcb *run_queue;

void sched_init(struct pcb *first) {
    proc_enqueue(&run_queue, first);
}

void sched_block(struct pcb *current) {
    struct pcb *next = run_queue->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
}

void sched_yield(void) {
    struct pcb *current = run_queue->next;
    run_queue = current;
    sched_block(current);
}

void sched_run(void (*fn)(void), struct rect area) {
    struct pcb *current = run_queue->next;
    struct pcb *pcb = proc_create(area);
    proc_enqueue(&run_queue, pcb);
    ctx_start(&current->sp, (struct page *) pcb + 1, fn);
}

void sched_idle() {
    for (;;) {
        sched_yield();
    }
}
