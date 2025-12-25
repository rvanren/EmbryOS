#pragma once

#include "types.h"

#define STIE     5      // Enable Supervisor Timer Interrupts

// This enables timer interrupts.  However, note that interrupts are still
// disabled globally. To enable those you have to set the SIE bit in the
// sstatus register.
static inline void intr_init(void) {
    asm volatile ("csrs sie, %0" :: "r"((uword_t) 1 << STIE));
}
