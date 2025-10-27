#pragma once

#include "syscall.h"
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
