#pragma once

#include "process.h"

// The run queue is a circular linked list.
extern struct pcb *run_queue;

// Initialize that schedule module with the first process
void sched_init(struct pcb *first);

// The current process pcb is replaced by the next process.
void sched_block(struct pcb *pcb);

// The current process voluntarily yields to another.
void sched_yield(void);

// Start a new process with the given initial function and screen area.
void sched_run(void (*fn)(void), struct rect area);

// This function is called when everything's idle.
void sched_idle();
