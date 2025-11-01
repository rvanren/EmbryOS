#pragma once

#include <stdint.h>

// ----------------------------------------------------------------------
//  Minimal legacy SBI console interface for OpenSBI v1.3 and earlier.
// ----------------------------------------------------------------------

// print one character
static inline void sbi_putchar(int ch) {
    register sword_t a0 asm("a0") = (unsigned char) ch;
    register sword_t a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

static inline void sbi_set_timer(uword_t next_time) {
#if __riscv_xlen == 32
    register uword_t a0 asm("a0") = (uword_t)next_time;
    register uword_t a1 asm("a1") = (uword_t)(next_time >> 32);
    asm volatile ("ecall" :: "r"(a0), "r"(a1), "r"(0), "r"(0x54494D45) : "memory");
#else
    register uword_t a0 asm("a0") = next_time;
    asm volatile ("ecall" :: "r"(a0), "r"(0), "r"(0x54494D45) : "memory");
#endif
}
