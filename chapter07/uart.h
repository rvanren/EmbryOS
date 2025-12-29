#pragma once

// A UART (Universal Asynchronous Receiver Transmitter) is used for
// input/output to simple serial peripherals (external devices) like
// a keyboard, mouse, or the screen.

struct uart_info {
    const char *type;                        // device type
    void (*init)(uintptr_t base);            // to initialize the device
    void (*putchar)(uintptr_t base, char c); // output a character
    void (*isr)(uintptr_t base);             // check for input
    uintptr_t base;                          // address of device registers
};

extern struct uart_info *uart;       // primary UART

void uart_init(const char *compat, int len, uintptr_t base);
