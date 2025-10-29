#pragma once

#include "process.h"

// There are three priorities (0 being the highest):
//  0: interactive processes, which are new processes or processes who just
//     woke up from having been blocked on input
//  1: background processes, which are processes that have been pre-empted
//  2: the "main" process that waits for interrupts in case everything else is idle

// Initialize that schedule module with the first process
void sched_init(struct pcb *first);

// Get me
struct pcb *sched_self();

// Move the current process to the given wait queue.
void sched_wait(struct pcb **q);

// The current process voluntarily yields to another. If its priority was 0,
// it is moved to run_queue[1].
void sched_yield(void);

// Start a new process with the given executable, screen area, and arguments.
void sched_run(int executable, struct rect area, void *args, int size);

// Kill the current process
void sched_exit(void);

// This function is called when everything's idle.  It waits for the next
// interrupt.
void sched_idle();
