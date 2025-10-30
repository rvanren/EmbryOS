#pragma once

// ----------------------------------------------------------------------
//  Minimal legacy SBI console interface for OpenSBI v1.3 and earlier.
//  Works on QEMU’s built-in firmware (-bios default).
// ----------------------------------------------------------------------

// print one character
static inline void sbi_putchar(int ch) {
    register long a0 asm("a0") = (unsigned char)ch;
    register long a7 asm("a7") = 1;   // SBI_CONSOLE_PUTCHAR (legacy)
    asm volatile ("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

// read one character (blocking)
static inline int sbi_getchar(void) {
    register long a7 asm("a7") = 2;   // SBI_CONSOLE_GETCHAR (legacy)
    register long ret asm("a0");
    asm volatile ("ecall" : "=r"(ret) : "r"(a7) : "memory");
    return (int)ret;
}
