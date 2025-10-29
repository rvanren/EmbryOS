#pragma once

#include "syscall.h"
#include "process.h"
#include "sched.h"
#include "uart.h"
#include "kprintf.h"
#include "interrupt.h"

static inline void user_exit() {
    intr_disable();
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("process ended<");
    sched_exit();
    intr_enable();      // should never be called
}

static inline void user_spawn(int app, int row, char col, int wd, int ht, const void *args, int sz) {
    intr_disable();
    sched_run(app, (struct rect){ row, col, wd, ht }, 0, 0);
    intr_enable();
}

static inline void user_put(int row, int col, cell_t cell) {
    intr_disable();
    struct pcb *self = sched_self();
    proc_put(self, row, col, cell);
    intr_enable();
}

static inline void user_yield() {
    intr_disable();
    sched_yield();
    intr_enable();
}
