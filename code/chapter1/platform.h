#pragma once

#if __riscv_xlen == 64
typedef int64_t  sword_t;   // signed machine word
typedef uint64_t uword_t;   // unsigned machine word
#else
typedef int32_t  sword_t;   // signed machine word
typedef uint32_t uword_t;   // unsigned machine word
#endif

// Platform-dependent constants go here

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
