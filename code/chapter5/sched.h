#ifndef SCHED_H
#define SCHED_H

#include "process.h"

#define N_PRIORITIES 3

extern struct pcb *run_queue[N_PRIORITIES];
extern int proc_current;

void sched_init(struct pcb *first);
void sched_yield(void);
void sched_run(entry_t fn, struct rect area);

#endif // SCHED_H
