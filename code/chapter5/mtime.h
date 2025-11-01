#include "platform.h"   // for uword_t, etc.

static inline uint64_t mtime_get(void) {
#if __riscv_xlen == 64
    uint64_t t;
    asm volatile ("csrr %0, time" : "=r"(t));
    return t;
#else
    uint32_t lo, hi, hi2;
    do {
        asm volatile ("csrr %0, timeh" : "=r"(hi));
        asm volatile ("csrr %0, time"  : "=r"(lo));
        asm volatile ("csrr %0, timeh" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
#endif
}
