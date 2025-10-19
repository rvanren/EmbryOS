#pragma once

#include "process.h"

// There are three priorities (0 being the highest):
//  0: interactive processes, which are new processes or processes who just woke up
//     from having been blocked on input
//  1: background processes, which are processes that have been pre-empted
//  2: the "main" process that waits for interrupts in case everything else is idle
#define N_PRIORITIES 3

// There is a circular queue for each priority.
extern struct pcb *run_queue[N_PRIORITIES];

// Maintains the priority of the process that is currently running.
// Its PCB is run_queue[proc_current]->next.  run_queue[proc_current] points
// to the last process that ran at that priority.
extern int proc_current;

// Initialize that schedule module with the first process
void sched_init(struct pcb *first);

// The current process pcb is replaced by the highest priority process.
void sched_block(struct pcb *pcb);

// The current process voluntarily yields to another. If its priority was 0,
// it is moved to run_queue[1].
void sched_yield(void);

// Start a new process with the given initial function and screen area.
void sched_run(void (*fn)(void), struct rect area);

// This function is called when everything's idle.  It waits for the next
// interrupt.
void sched_idle();
