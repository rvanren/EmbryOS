#include "sched.h"
#include "ctx.h"
#include "process.h"
#include "platform.h"

#define N_PRIORITIES 3

int current_priority;
struct pcb *run_queue[N_PRIORITIES];
static struct pcb *zombies;   // list of PCBs pending free

void sched_init(struct pcb *first) {
    proc_enqueue(&run_queue[0], first);
}

struct pcb *sched_self() { return run_queue[current_priority]->next; }

static void reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        proc_release(pcb);
    }
}

void sched_block(struct pcb *current) {
    current_priority = 0;
    while (current_priority < N_PRIORITIES && run_queue[current_priority] == 0)
        current_priority++;
    struct pcb *next = run_queue[current_priority]->next;
    if (next != current) ctx_switch(&current->sp, next->sp);
    reap_zombies();
}

void sched_run(int executable, struct rect area, void *args, int size) {
    struct pcb *current = sched_self();
    struct pcb *pcb = proc_create(executable, area, args, size);
    proc_enqueue(&run_queue[0], pcb);
    current_priority = 0;
    ctx_start(&current->sp, (char *) pcb + PAGE_SIZE);
    reap_zombies();
}

void sched_exit(void) {
    struct pcb *pcb = proc_dequeue(&run_queue[current_priority]);
    pcb->next = zombies;
    zombies = pcb;
    sched_block(pcb);
}
