#pragma once

// Platform-dependent constants go here

#ifdef SIFIVE

#define UART_BASE       0x10010000UL
#define UART_SIFIVE

#endif

#ifdef VIRT

#define UART_BASE       0x10000000UL
#define UART_16550

#endif

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
