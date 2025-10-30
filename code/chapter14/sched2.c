#include "sched.h"
#include "process.h"
#include "interrupt.h"

extern int current_priority;
extern struct pcb *run_queue[];

void sched_wait(struct pcb **q) {
    struct pcb *me = proc_dequeue(&run_queue[current_priority]);
    proc_enqueue(q, me);
    sched_block(me);
}

void sched_resume(struct pcb *pcb) {
    proc_enqueue(&run_queue[0], pcb);
}

void sched_yield(void) {
    if (current_priority == 0) {
        struct pcb *me = proc_dequeue(&run_queue[0]);
        proc_enqueue(&run_queue[1], me);
        current_priority = 1;
    }
    struct pcb *current = run_queue[current_priority]->next;
    run_queue[current_priority] = current;
    sched_block(current);
}

void sched_idle() {
    struct pcb *pcb = proc_dequeue(&run_queue[current_priority]);
    proc_enqueue(&run_queue[2], pcb);
    current_priority = 2;
    for (;;) {
        sched_yield();
        intr_enable(); __asm__ volatile ("wfi"); intr_disable();
    }
}
