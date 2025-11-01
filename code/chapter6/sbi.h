#pragma once

#include "platform.h"

static inline void sbi_putchar(int ch) {
    register sword_t a0 asm("a0") = (unsigned char) ch;
    register sword_t a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

static inline void sbi_set_timer(uint64_t next_time) {
#if __riscv_xlen == 32
    register uword_t a0 asm("a0") = (uword_t)next_time;
    register uword_t a1 asm("a1") = (uword_t)(next_time >> 32);
    register uword_t a6 asm("a6") = 0;                 // function ID
    register uword_t a7 asm("a7") = 0x54494D45UL;      // "TIME"
    asm volatile ("ecall" : "+r"(a0), "+r"(a1) : "r"(a6), "r"(a7) : "memory");
#else
    register uword_t a0 asm("a0") = (uword_t)next_time;
    register uword_t a6 asm("a6") = 0;                 // function ID
    register uword_t a7 asm("a7") = 0x54494D45UL;      // "TIME"
    asm volatile ("ecall" : "+r"(a0) : "r"(a6), "r"(a7) : "memory");
#endif
}
