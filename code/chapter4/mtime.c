#include "interrupt.h"

#define CLINT_BASE       0x02000000UL

#define MTIME_ADDR       (CLINT_BASE + 0xBFF8)
#define MTIME_CMP(hart)  (CLINT_BASE + 0x4000 + 8 * (hart))

#define MTIE_MASK        (1u << 7)

void mtime_init() {
    asm("csrs mie, %0"::"r"(MTIE_MASK)); // set MTIE=1, unmask timer interrupts
}

long long mtime_get() {
    return *((long long *) MTIME_ADDR);
}

void mtime_reset(long long quantum) {
    *((long long *) MTIME_CMP(1)) = mtime_get() + quantum;
}
