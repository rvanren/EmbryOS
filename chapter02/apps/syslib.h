#pragma once

#include "syscall.h"
#include "platform.h"
#include "screen.h"
#include "log.h"

static inline void user_put(int col, int row, cell_t cell) {
    L3(L_FREQ, L_USER_PUT, col, row, cell);
    screen_put(col, row, cell);
}

static inline void user_delay(int ms) {  // pseudo system call
    L1(L_NORM, L_USER_DELAY, ms);
    while (--ms > 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}
