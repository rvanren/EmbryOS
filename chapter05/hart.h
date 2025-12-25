#pragma once

#include "types.h"

// A "hart" is a RISC-V core.  Per hart, we maintain various info.
struct hart {
    unsigned int idx;                // index 0, 1, 2, ...
    uword_t id;                      // actual hart id
    int interrupts_work;             // non-zero if supports (timer) interrupts
};
