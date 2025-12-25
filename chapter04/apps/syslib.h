#pragma once

#include "syscall.h"
#include "platform.h"
#include "process.h"
#include "sched.h"
#include "io.h"
#include "kprintf.h"
#include "interrupt.h"
#include "log.h"

// Bit 1 = SIE (Supervisor Interrupt Enable)
#define SIE_MASK ((uword_t) 1 << 1)

static inline void intr_enable(void) {
    __asm__ volatile ("csrs sstatus, %0" :: "r"(SIE_MASK));
}

static inline void intr_disable(void) {
    __asm__ volatile ("csrc sstatus, %0" :: "r"(SIE_MASK));
}

static inline void user_yield() {
    intr_disable();
    L0(L_NORM, L_USER_YIELD);
    sched_yield();
    intr_enable();
}

static inline void user_spawn(int app, int row, char col, int wd, int ht, const void *args, int sz) {
    intr_disable();
    L1(L_NORM, L_USER_SPAWN, app);
    sched_run(app, (struct rect){ row, col, wd, ht }, 0, 0);
    intr_enable();
}

static inline void user_put(int row, int col, cell_t cell) {
    intr_disable();
    L3(L_FREQ, L_USER_PUT, row, col, cell);
    proc_put(sched_self(), row, col, cell);
    intr_enable();
}

static inline void user_delay(int ms) {  // pseudo system call
    user_yield();
    while (--ms > 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}
