#pragma once

#include "process.h"

#define N_PRIORITIES 3

extern struct pcb *run_queue[N_PRIORITIES];
extern int proc_current;

void sched_init(struct pcb *first);
void sched_block(struct pcb *current);
void sched_yield(void);
void sched_run(void (*fn)(void), struct rect area);
void sched_idle();
