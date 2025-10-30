#include <stdint.h>
#include "interrupt.h"
#include "platform.h"

#define MTIME_ADDR       (CLINT_BASE + 0xBFF8)
#define MTIME_CMP(hart)  (CLINT_BASE + 0x4000 + 8 * (hart))

#define MTIE_MASK        (1u << 7)

static inline uint32_t mread_hartid(void) {
    uint32_t hart;
    asm volatile ("csrr %0, mhartid" : "=r"(hart));
    return hart;
}

void mtime_init(void) {
    asm volatile ("csrs mie, %0" :: "r"(MTIE_MASK));  // enable timer interrupt
}

uint64_t mtime_get(void) {
    return *((volatile uint64_t *) MTIME_ADDR);
}

void mtime_reset(uint64_t quantum) {
    uint32_t hart = mread_hartid();
    volatile uint64_t *cmp = (volatile uint64_t *) MTIME_CMP(hart);
    *cmp = mtime_get() + quantum;
}
