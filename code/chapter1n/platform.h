#pragma once

// Platform-dependent constants go here

#define UART_BASE       0x10010000UL
#define UART_IRQ        4

#define PLIC_BASE       0x0C000000UL
#define CLINT_BASE      0x02000000UL

#define DELAY_MS        10000000

// #define NO_PMP       // define if no PMP
