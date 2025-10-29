#pragma once

#include "process.h"

// Initialize that schedule module with the first process
void sched_init(struct pcb *first);

// Get me
struct pcb *sched_self();

// The current process pcb is replaced
void sched_block(struct pcb *pcb);

// The current process voluntarily yields to another
void sched_yield(void);

// Start a new process with the given executable, screen area, and arguments.
void sched_run(int executable, struct rect area, void *args, int size);

// Kill the current process
void sched_exit(void);

// This function is called when everything's idle.  It waits for the next
// interrupt.
void sched_idle();
