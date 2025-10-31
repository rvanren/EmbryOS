#include "trap.h"

// Platform-Level Interrupt Controller interface.  It
//  - collects all interrupt sources from peripherals
//  - prioritizes and masks them
//  - delivers them to the right hart (core)
//  - allows the OS to acknowledge ("complete") them.

// This function is called when there is an external interrupt
void plic_handler(struct trap_frame *tf);

// Initializes the plic module
void plic_init(uint32_t hartid);
