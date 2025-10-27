#pragma once

#include "syscall.h"

#ifdef CH7

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

#ifdef CH6
static inline int user_get(int row, int col, cell_t cursor_f, cell_t cursor_u) {
    register int a7 asm("a7") = SYS_GET;
    register int a0 asm("a0") = row;
    register int a1 asm("a1") = col;
    register int a2 asm("a2") = cursor_f;
    register int a3 asm("a3") = cursor_u;
    asm volatile("ecall" : "=r"(a0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a7));
    return a0;
}
#endif

#ifdef CH12

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

#endif

#else

#include "process.h"
#include "sched.h"
#include "uart.h"
#include "kprintf.h"
#include "interrupt.h"

static inline void user_exit() {
    intr_disable();
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("process ended<");
    proc_exit();
    intr_enable();      // should never be called
}

static inline void user_spawn(int app, int row, char col, int wd, int ht, const void *args, int sz) {
    intr_disable();
    sched_run(app, (struct rect){ row, col, wd, ht }, 0, 0);
    intr_enable();
}

static inline void user_put(int row, int col, cell_t cell) {
    intr_disable();
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, row, col, cell);
    intr_enable();
}

static inline int user_get(int row, int col, cell_t cursor_f, cell_t cursor_u) {
    intr_disable();
    struct pcb *self = run_queue[proc_current]->next;
    int c = uart_get(self, row, col, cursor_f, cursor_u);
    intr_enable();
    return c;
}

#endif
