#pragma once

// Platform-dependent constants go here

#if __riscv_xlen == 32
#define VBITS 32            // # virtual address bits
#define VM_END          0x70400000        // 1024 pages
#endif
#if __riscv_xlen == 64
#define VBITS 39            // # virtual address bits
#define VM_END          0x70200000        // 512 pages
#endif

#define PAGE_SHIFT      12
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#define VM_START        0x70000000

#ifndef DELAY_MS
#define DELAY_MS        326087	// 1 ms of delay (see syslib.h)
#endif
