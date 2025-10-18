#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"

struct pcb *run_queue = 0;

void sched_init(struct pcb *first) {
    proc_enqueue(&run_queue, first);
}

void sched_yield(void) {
    struct pcb *current = run_queue->next;
    run_queue = current;
    struct pcb *next = run_queue->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
}

void sched_run(entry_t fn, struct rect area) {
    struct pcb *current = run_queue->next;
    struct pcb *pcb = proc_create(area);
    proc_enqueue(&run_queue, pcb);
    ctx_start(&current->sp, (struct page *) pcb + 1, fn);
}
