#ifdef CH4

#pragma once

#include "trap.h"

// All interrupt handlers have this type.  They are called with a pointer to
// a trap frame that contains all the saved registers.
typedef void (*trap_entry_t)(struct trap_frame *);

// These are the classes of interrupts
enum intr_class { INTR_TIMER, INTR_EXTERNAL, INTR_SYSCALL, INTR_EXCEPTION };

void intr_enable(void);     // enable interrupts
void intr_disable(void);    // disable interrupts
int intr_init();            // initialize this module

// Set the handler for the given type of interrupt
void intr_set_handler(enum intr_class which, trap_entry_t handler);

#endif
