#pragma once

// A UART (Universal Asynchronous Receiver Transmitter) is used for input/output to
// simple serial peripherals (external devices) like a keyboard, mouse, or the screen.

void putchar(char c);   // write a character to the screen and update the cursor
void uart_init(void);   // initialize this module
