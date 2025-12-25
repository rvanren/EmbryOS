#pragma once

#include "types.h"

// A "hart" is a RISC-V core.  Per hart, we maintain various info.
struct hart {
    unsigned int idx;                // index 0, 1, 2, ...
    uword_t id;                      // actual hart id
    uword_t *parent_page_table;      // see vm*.c
    int needs_tlb_flush;             // non-zero if TLB flush needed
    int interrupts_work;             // non-zero if supports (timer) interrupts
};
