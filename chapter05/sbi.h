#pragma once

#include "types.h"

// The Supervisor Binary Interface (SBI) provides a set of interfaces
// to S-mode kernels to request services of M-mode firmware like OpenSBI.

// Output character ch to the primary UART
extern void sbi_putchar(int ch);

// Schedule a timer interrupt
extern void sbi_set_timer(uint64_t next_time);

// Not technically part of the SBI, this function returns the current
// "machine time" in ticks.
extern uint64_t mtime_get(void);

// Start another hart (core), identified by hartid.  It calls a function
// start_addr(opaque).
extern sword_t sbi_hart_start(uword_t hartid,
                uword_t start_addr, uword_t opaque);
