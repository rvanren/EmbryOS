#pragma once

// Platform-dependent constants go here

#define PAGE_SHIFT      13
#define PAGE_SIZE       (1 << PAGE_SHIFT)

#ifdef SIFIVE

#define UART_BASE       0x10010000UL
#define UART_SIFIVE

#endif

#ifdef VIRT

#define UART_BASE       0x10000000UL
#define UART_16550

// #define NO_PMP       // define if no PMP

#endif

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
