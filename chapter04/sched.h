#pragma once

#include "process.h"

// Function for a live process to clean up dead ones.
void reap_zombies(void);

// Put the given process back on the run queue
void sched_resume(struct pcb *pcb);

// Find a new process to run.
void sched_block(struct pcb *old);

// The current process voluntarily yields to another. If its priority was 0,
// it is moved to run_queue[1].
void sched_yield(void);

// Start a new process with the given executable, screen area, and arguments.
void sched_run(int executable, struct rect area, void *args, int size);

// Idle loop
void sched_idle();

// Kill the current process
void sched_exit(void);

// Set 'self', which is maintained in the tp register
static inline void sched_set_self(struct pcb *ptr) {
    asm volatile ("mv tp, %0" :: "r"(ptr));
}

// Get 'self' from the tp register
static inline struct pcb *sched_self(void) {
    void *val;
    asm volatile ("mv %0, tp" : "=r"(val));
    return val;
}
