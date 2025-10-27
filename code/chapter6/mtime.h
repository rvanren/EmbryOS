#include <stdint.h>

// Initialize the RISC-V machine timer
void mtime_init();

// Return the value of the current MTIME register, in microseconds
uint64_t mtime_get();

// Request a timer interrupt after the given number of microseconds
void mtime_reset(uint64_t quantum);
