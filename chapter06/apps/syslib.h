#pragma once

#include "syscall.h"
#include "platform.h"

extern void user_exit();
extern void user_yield();
extern void user_spawn(int app, int col, int row, int wd, int ht, const void *args, int sz);
extern void user_put(int col, int row, cell_t cell);
extern int user_get(int block);

static inline void user_delay(int ms) {  // pseudo system call
    user_yield();
    while (--ms > 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}
