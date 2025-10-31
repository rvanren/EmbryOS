#pragma once

#include <stdint.h>

// ----------------------------------------------------------------------
//  Minimal legacy SBI console interface for OpenSBI v1.3 and earlier.
// ----------------------------------------------------------------------

// print one character
static inline void sbi_putchar(int ch) {
    register long a0 asm("a0") = (unsigned char)ch;
    register long a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR (legacy)
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

static inline void sbi_set_timer(uint64_t next_time) {
#if __riscv_xlen == 32
    register uint32_t a0 asm("a0") = (uint32_t)next_time;
    register uint32_t a1 asm("a1") = (uint32_t)(next_time >> 32);
    register uint32_t a6 asm("a6") = 0;              // function ID = 0
    register uint32_t a7 asm("a7") = 0x54494D45;     // "TIME"
    asm volatile ("ecall"
                  : "+r"(a0), "+r"(a1)
                  : "r"(a6), "r"(a7)
                  : "memory");
#else
    register uint64_t a0 asm("a0") = next_time;
    register uint64_t a6 asm("a6") = 0;
    register uint64_t a7 asm("a7") = 0x54494D45;     // "TIME"
    asm volatile ("ecall"
                  : "+r"(a0)
                  : "r"(a6), "r"(a7)
                  : "memory");
#endif
}
