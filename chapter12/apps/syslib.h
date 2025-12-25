#pragma once

#include "syscall.h"
#include "platform.h"

extern void user_exit();
extern void user_yield();
extern void user_spawn(int app, int row, char col, int wd, int ht, const void *args, int sz);
extern void user_put(int row, int col, cell_t cell);
extern int user_get(int block);
extern int user_create(void);
extern int user_read(int file, int off, void *dst, int n);
extern int user_write(int file, int off, const void *src, int n);
extern int user_size(int file);
extern void user_delete(int file);

static inline void user_delay(int ms) {  // pseudo system call
    user_yield();
    while (--ms > 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}
