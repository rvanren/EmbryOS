#pragma once
#include "trap.h"
#include "platform.h"   // for uword_t

// All interrupt handlers have this type.  They are called with a pointer to
// a trap frame that contains all the saved registers.
typedef void (*trap_entry_t)(struct trap_frame *);

// Classes of interrupts
enum intr_class { INTR_TIMER, INTR_EXTERNAL, INTR_SYSCALL, INTR_EXCEPTION };

// Bit 1 = SIE (Supervisor Interrupt Enable)
#define SIE_MASK ((uword_t)1 << 1)

static inline void intr_enable(void)  { __asm__ volatile ("csrs sstatus, %0" :: "r"(SIE_MASK)); }
static inline void intr_disable(void) { __asm__ volatile ("csrc sstatus, %0" :: "r"(SIE_MASK)); }

int  intr_init(void);  // initialize this module
void intr_set_handler(enum intr_class which, trap_entry_t handler);
