#pragma once

// This is the system call interface that user processes use.  For efficiency reasons,
// the interface is inlined (implemented right here).

#include "syscall.h"

// Write character c at the given position and in the given foreground and background
// colors.  Also moves the cursor to the next position.
static inline void user_put(int row, int col, char c, int fg, int bg) {
    register int a7 asm("a7") = SYS_PUT;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = c;
    register int a3 asm("a3") = fg;
    register int a4 asm("a4") = bg;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a7));
}

// Return the next typed character.
static inline int user_get(void) {
    register int a7 asm("a7") = SYS_GET;
    register int a0 asm("a0");
    asm volatile("ecall" : "=r"(a0) : "r"(a7));
    return a0;
}
