#pragma once

#include "process.h"
#include "syscall.h"

// A UART (Universal Asynchronous Receiver Transmitter) is used for input/output to
// simple serial peripherals (external devices) like a keyboard, mouse, or the screen.

void putchar(char c);   // write a character to the screen and update the cursor
void uart_init(void);   // initialize this module
int uart_get(struct process *c, int row, int col, cell_t cf, cell_t cu) {
void uart_isr();        // the UART interrupt service routine
