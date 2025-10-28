#pragma once

// Platform-dependent constants go here

#define UART_BASE       0x10010000UL
#define UART_IRQ        4

#define PLIC_BASE       0x0C000000UL
#define CLINT_BASE      0x02000000UL

#define HART_ID         1   // hart ID of the core that runs the kernel
#define HART_CTX        1   // its S-mode context

#define DELAY_MS        326087	// 1 ms of delay (see syscall.h)

// #define NO_PMP       // define if no PMP
