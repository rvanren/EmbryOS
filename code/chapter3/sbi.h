#pragma once

#include "platform.h"

static inline void sbi_putchar(int ch) {
    register sword_t a0 asm("a0") = (unsigned char) ch;
    register sword_t a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}
