#pragma once

// Here is the current system call interface

#define SYS_PUT 0       // write a character to the screen
#define SYS_GET 1       // read a character from the keyboard

#include "interrupt.h"

// Initialize the syscall module
void syscall_init(void);

// This handler is invoked on a system call (ecall).
void syscall_handler(struct trap_frame *);
