#pragma once

// Platform-dependent constants go here

#ifdef SIFIVE

#define UART_BASE       0x10010000UL
#define UART_SIFIVE
#define UART_IRQ        4

#define PLIC_BASE       0x0C000000UL
#define CLINT_BASE      0x02000000UL

#define HART_CTX        1

// #define NO_PMP       // define if no PMP

#endif

#ifdef VIRT

#define UART_BASE       0x10000000UL
#define UART_16550
#define UART_IRQ        10

#define PLIC_BASE       0x0C000000UL
#define CLINT_BASE      0x02000000UL

#define HART_CTX        2

// #define NO_PMP       // define if no PMP

#endif

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)
