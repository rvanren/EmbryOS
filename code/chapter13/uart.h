#pragma once

// A UART (Universal Asynchronous Receiver Transmitter) is used for input/output to
// simple serial peripherals (external devices) like a keyboard, mouse, or the screen.

#include "process.h"
#include "syscall.h"

void uart_received(char c);       // somebody typed c
void uart_put(struct pcb *pcb, cell_t c);
void uart_isr();                  // the UART interrupt service routine
void uart_exit(struct pcb *pcb);  // called when process exits
void uart_init(void);             // initialize this module
void uart_putchar(char c);        // write c to the screen
int uart_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu);
