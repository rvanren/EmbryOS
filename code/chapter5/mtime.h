#include <stdint.h>

static inline uint64_t mtime_get(void) {
    uint32_t lo, hi, hi2;
    do {
        asm volatile ("rdtimeh %0" : "=r"(hi));
        asm volatile ("rdtime  %0" : "=r"(lo));
        asm volatile ("rdtimeh %0" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t) hi << 32) | lo;
}
