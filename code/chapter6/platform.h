#pragma once

// Platform-dependent constants go here

#define PAGE_SHIFT      13
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#ifdef SIFIVE

#define UART_SIFIVE
#define UART_BASE       0x10010000UL
#define UART_IRQ        4

#endif

#ifdef VIRT

#define UART_16550
#define UART_BASE       0x10000000UL
#define UART_IRQ        10

#endif

#define PLIC_BASE       0x0C000000UL

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
