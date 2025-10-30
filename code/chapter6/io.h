#pragma once

#include "process.h"
#include "syscall.h"

void io_received(char c);       // somebody typed c
void io_put(struct pcb *pcb, cell_t c);
void io_exit(struct pcb *pcb);  // called when process exits
void io_putchar(char c);        // write c to the screen
int  io_get(struct pcb *self, int row, int col, cell_t cf, cell_t cu);
