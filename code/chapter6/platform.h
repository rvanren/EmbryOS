#pragma once

// Platform-dependent constants go here

#define PAGE_SHIFT      13
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#define UART_16550

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
