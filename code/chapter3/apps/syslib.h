#pragma once

#include "syscall.h"
#include "screen.h"

static inline void user_put(int row, int col, cell_t cell) {
    screen_put(row, col, cell);
}
