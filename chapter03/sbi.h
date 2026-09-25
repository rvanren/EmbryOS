#pragma once

#include "types.h"

// The Supervisor Binary Interface (SBI) provides a set of interfaces
// to S-mode kernels to request services of M-mode firmware like OpenSBI.

// Output character ch to the primary UART
extern void sbi_putchar(int ch);

// Not technically part of the SBI, this function returns the current
// "machine time" in ticks.
extern uint64_t mtime_get(void);
