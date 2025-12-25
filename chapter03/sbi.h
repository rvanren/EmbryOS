#pragma once

#include "types.h"

// The Supervisor Binary Interface (SBI) provides a set of interfaces
// to S-mode kernels to request services of M-mode firmware like OpenSBI.

// Output character ch to the primary UART
extern void sbi_putchar(int ch);
