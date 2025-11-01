#pragma once

#include <stdint.h>

#if __riscv_xlen == 64
typedef int64_t  sword_t;   // signed machine word
typedef uint64_t uword_t;   // unsigned machine word
#else
typedef int32_t  sword_t;   // signed machine word
typedef uint32_t uword_t;   // unsigned machine word
#endif

// Platform-dependent constants go here

#define PAGE_SHIFT      13
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#ifdef SIFIVE

#define UART_SIFIVE
#define UART_BASE       0x10010000ULL
#define UART_IRQ        4

#define PLIC_CTX(hart)       (2 * (hart))

#endif

#ifdef VIRT

#define UART_16550
#define UART_BASE       0x10000000ULL
#define UART_IRQ        10

#define PLIC_CTX(hart)       (2 * (hart) + 1)

#endif

#define PLIC_BASE       0x0C000000UL

#define DELAY_MS        326087    // 1 ms of delay (see syscall.h)
