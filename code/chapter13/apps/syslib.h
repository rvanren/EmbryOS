#pragma once

#include "syscall.h"

static inline void user_exit() {
    register int a7 asm("a7") = SYS_EXIT;
    asm volatile("ecall" : : "r"(a7));
}

static inline void user_spawn(int app, int row, char col, int wd, int ht, const void *args, int sz) {
    register int a7 asm("a7") = SYS_SPAWN;
    register int a0 asm("a0") = app;
    register int a1 asm("a1") = row;
    register int a2 asm("a2") = col;
    register int a3 asm("a3") = wd;
    register int a4 asm("a4") = ht;
    register const void *a5 asm("a5") = args;
    register int a6 asm("a6") = sz;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7));
}

static inline void user_put(int row, int col, cell_t cell) {
    register int a7 asm("a7") = SYS_PUT;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = cell;
    asm volatile("ecall" : : "r"(a0), "r"(a1), "r"(a2), "r"(a7));
}

static inline int user_get(int row, int col, cell_t cursor_f, cell_t cursor_u) {
    register int a7 asm("a7") = SYS_GET;
    register int a0 asm("a0");
    asm volatile("ecall" : "=r"(a0) : "r"(a7));
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = cursor_f;
    register int a3 asm("a3") = cursor_u;
    return a0;
}

static inline int user_create(void) {
    register int a7 asm("a7") = SYS_CREATE;
    register int a0 asm("a0");
    asm volatile("ecall" : "=r"(a0) : "r"(a7));
    return a0;
}

static inline int user_read(int file, int off, void *dst, int n) {
    register int a7 asm("a7") = SYS_READ;
    register int a0 asm("a0") = file;
    register int a1 asm("a1") = off;
    register void *a2 asm("a2") = dst;
    register int a3 asm("a3") = n;
    asm volatile("ecall" : "=r"(a0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a7));
    return a0;
}

static inline int user_write(int file, int off, const void *src, int n) {
    register int a7 asm("a7") = SYS_WRITE;
    register int a0 asm("a0") = file;
    register int a1 asm("a1") = off;
    register const void *a2 asm("a2") = src;
    register int a3 asm("a3") = n;
    asm volatile("ecall" : "=r"(a0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a7));
    return a0;
}

static inline int user_size(int file) {
    register int a7 asm("a7") = SYS_SIZE;
    register int a0 asm("a0") = file;
    asm volatile("ecall" : "=r"(a0) : "r"(a0), "r"(a7));
    return a0;
}

static inline void user_delete(int file) {
    register int a7 asm("a7") = SYS_DELETE;
    register int a0 asm("a0") = file;
    asm volatile("ecall" : : "r"(a0));
}
