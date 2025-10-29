#include "sched.h"
#include "ctx.h"
#include "frame.h"
#include "process.h"
#include "interrupt.h"

#define N_PRIORITIES 3

static struct pcb *run_queue[N_PRIORITIES];
static struct pcb *zombies = 0;   // list of PCBs pending free
static int current_priority;

void sched_init(struct pcb *first) {
    for (int i = 0; i < N_PRIORITIES; i++) run_queue[i] = 0;
    proc_enqueue(&run_queue[0], first);
    current_priority = 0;
}

struct pcb *sched_self() { return run_queue[current_priority]->next; }

static void reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        proc_release(pcb);
    }
}

void proc_exit(void) {
    struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
    pcb->next = zombies;
    zombies = pcb;
    sched_block(pcb);
}

void sched_block(struct pcb *current) {
    current_priority = 0;
    while (current_priority < N_PRIORITIES && run_queue[current_priority] == 0)
        current_priority++;
    struct pcb *next = run_queue[current_priority]->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
    reap_zombies();
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

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *current = sched_self();
    struct pcb *pcb = proc_create(executable, area, args, size);
    proc_enqueue(&run_queue[0], pcb);
    current_priority = 0;
    ctx_start(&current->sp, (char *) pcb + PAGE_SIZE);
    reap_zombies();
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
