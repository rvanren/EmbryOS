#pragma once

#include "syscall.h"
#include "platform.h"
#include "process.h"
#include "sched.h"
#include "kprintf.h"
#include "log.h"

static inline void user_yield() {
    L0(L_NORM, L_USER_YIELD);
    sched_yield();
}

static inline void user_spawn(int app, int col, char row, int wd, int ht, const void *args, int sz) {
    L1(L_NORM, L_USER_SPAWN, app);
    sched_run(app, (struct rect){ col, row, wd, ht }, 0, 0);
}

static inline void user_put(int col, int row, cell_t cell) {
    L3(L_FREQ, L_USER_PUT, col, row, cell);
    proc_put(sched_self(), col, row, cell);
}

static inline void user_delay(int ms) {  // pseudo system call
    L1(L_NORM, L_USER_DELAY, ms);
    user_yield();
    while (--ms > 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}
