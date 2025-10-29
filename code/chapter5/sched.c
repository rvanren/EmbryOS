#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"
#include "interrupt.h"

static struct pcb *run_queue, *zombies;

void sched_init(struct pcb *first) {
    proc_enqueue(&run_queue, first);
}

struct pcb *sched_self() { return run_queue->next; }

static void reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        proc_release(pcb);
    }
}

void sched_block(struct pcb *current) {
    struct pcb *next = run_queue->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
    reap_zombies();
}

void sched_yield(void) {
    struct pcb *current = run_queue->next;
    run_queue = current;
    sched_block(current);
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *current = sched_self();
    struct pcb *pcb = proc_create(executable, area, args, size);
    proc_enqueue(&run_queue, pcb);
    ctx_start(&current->sp, (char *) pcb + PAGE_SIZE);
    reap_zombies();
}

void sched_exit(void) {
    struct pcb *pcb = proc_dequeue(&run_queue);
    pcb->next = zombies;
    zombies = pcb;
    sched_block(pcb);
}

void sched_idle() {
    for (;;) {
        sched_yield();
        intr_enable(); __asm__ volatile ("wfi"); intr_disable();
    }
}
