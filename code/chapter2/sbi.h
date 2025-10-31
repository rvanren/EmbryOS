#pragma once

static inline void sbi_putchar(int ch) {
    register long a0 asm("a0") = (unsigned char)ch;
    register long a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR (legacy)
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}
