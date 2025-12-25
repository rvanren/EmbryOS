#pragma once 

#include <stdint.h>

typedef uint32_t spinlock_t;

static inline void spinlock_init(spinlock_t *lock) {
    *lock = 0;
}

static inline void spinlock_acquire(spinlock_t *lock) {
    uint32_t tmp;
    do asm volatile ("amoswap.w.aq %0, %1, (%2)" : "=r"(tmp)
            : "r"(1), "r"(lock) : "memory");
    while (tmp != 0);     // spin until previous value was 0
}

static inline void spinlock_release(spinlock_t *lock) {
    __atomic_store_n(lock, 0, __ATOMIC_RELEASE);
}
