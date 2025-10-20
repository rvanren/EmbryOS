#pragma once

#include "syscall.h"

static inline void user_exit() {
    register int a7 asm("a7") = SYS_EXIT;
    asm volatile("ecall" : : "r"(a7));
}

static inline void user_spawn(int app, int row, char col, int wd, int ht) {
    register int a7 asm("a7") = SYS_SPAWN;
    register int a0 asm("a0") = app;
    register int a1 asm("a1") = row;
    register int a2 asm("a2") = col;
    register int a3 asm("a3") = wd;
    register int a4 asm("a4") = ht;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a7));
}

static inline void user_put(int row, int col, char c, int fg, int bg) {
    register int a7 asm("a7") = SYS_PUT;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = c;
    register int a3 asm("a3") = fg;
    register int a4 asm("a4") = bg;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a7));
}

static inline void user_put(int row, int col, char c, int fg, int bg) {
    register int a7 asm("a7") = SYS_PUT;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = c;
    register int a3 asm("a3") = fg;
    register int a4 asm("a4") = bg;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a7));
}

static inline int user_get(void) {
    register int a7 asm("a7") = SYS_GET;
    register int a0 asm("a0");
    asm volatile("ecall" : "=r"(a0) : "r"(a7));
    return a0;
}
