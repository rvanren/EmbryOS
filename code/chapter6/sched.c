#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"
#include "interrupt.h"

struct pcb *run_queue[N_PRIORITIES];
int proc_current;

void sched_init(struct pcb *first) {
    for (int i = 0; i < N_PRIORITIES; i++) run_queue[i] = 0;
    proc_enqueue(&run_queue[0], first);
    proc_current = 0;
}

void sched_block(struct pcb *current) {
    proc_current = 0;
    while (proc_current < N_PRIORITIES && run_queue[proc_current] == 0)
        proc_current++;
    struct pcb *next = run_queue[proc_current]->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
}

void sched_yield(void) {
    if (proc_current == 0) {
        struct pcb *me = proc_dequeue(&run_queue[0]);
        proc_enqueue(&run_queue[1], me);
        proc_current = 1;
    }
    struct pcb *current = run_queue[proc_current]->next;
    run_queue[proc_current] = current;
    sched_block(current);
}

void sched_run(void (*fn)(void), struct rect area) {
    struct pcb *current = run_queue[proc_current]->next;
    struct pcb *pcb = proc_create(area);
    proc_enqueue(&run_queue[0], pcb);
    proc_current = 0;
    ctx_start(&current->sp, (struct page *) pcb + 1, fn);
}

void sched_idle() {
    struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
    proc_enqueue(&run_queue[2], pcb);
    proc_current = 2;
    for (;;) {
        sched_yield();
        intr_enable();
        __asm__ volatile ("wfi");  // wait-for-interrupt
        intr_disable();
    }
}
