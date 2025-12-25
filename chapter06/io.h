#pragma once

#include "process.h"
#include "syscall.h"

// Called when somebody typed character c
void io_received(char c);

// Put cell c at the the location where the process is now expecting
// input.
void io_put(struct pcb *pcb, cell_t c);

// When a process exits, it loses its input focus if any
void io_exit(struct pcb *pcb);

// Send character c to the UART
void io_putchar(char c);

// Process pcb wants a character.  If block is non-zero, the process
// would like to wait for it if there isn't any.
int  io_get(struct pcb *pcb, int block);

// Add a character to the process's input buffer
void io_add(struct pcb *pcb, int c);
