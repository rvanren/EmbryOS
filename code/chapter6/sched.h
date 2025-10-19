#ifndef SCHED_H
#define SCHED_H

#include "process.h"
#include "trap.h"

#define N_PRIORITIES 3

extern struct pcb *run_queue[N_PRIORITIES];
extern int proc_current;

void sched_init(struct pcb *first);
void sched_block(struct pcb *current);
void sched_yield(void);
void sched_run(entry_t fn, struct rect area);
void sched_idle();

#endif // SCHED_H
