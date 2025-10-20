#pragma once

#define SYS_EXIT  0     // exit process
#define SYS_SPAWN 1     // start a new process
#define SYS_GET   2     // read a character from the keyboard
#define SYS_PUT   3     // write a character to the screen

#include "interrupt.h"

// This handler is invoked on a system call (ecall).
void syscall_handler(struct trap_frame *);
