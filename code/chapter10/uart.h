#pragma once

// A UART (Universal Asynchronous Receiver Transmitter) is used for input/output to
// simple serial peripherals (external devices) like a keyboard, mouse, or the screen.

void uart_isr();                  // the UART interrupt service routine
void uart_init(void);             // initialize this module
