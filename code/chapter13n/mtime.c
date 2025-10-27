#include "interrupt.h"
#include "platform.h"

#define MTIME_ADDR       (CLINT_BASE + 0xBFF8)
#define MTIME_CMP(hart)  (CLINT_BASE + 0x4000 + 8 * (hart))

#define MTIE_MASK        (1u << 7)

void mtime_init() {
    asm("csrs mie, %0"::"r"(MTIE_MASK)); // set MTIE=1, unmask timer interrupts
}

uint64_t mtime_get() {
    return *((volatile uint64_t *) MTIME_ADDR);
}

void mtime_reset(uint64_t quantum) {
    *((volatile uint64_t *) MTIME_CMP(1)) = mtime_get() + quantum;
}
