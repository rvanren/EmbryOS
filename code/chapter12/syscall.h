#pragma once

#define SYS_EXIT    0     // exit process
#define SYS_SPAWN   1     // start a new process
#define SYS_GET     2     // read a character from the keyboard
#define SYS_PUT     3     // write a character to the screen

#define SYS_CREATE  4     // create a new file
#define SYS_READ    5     // read a file
#define SYS_WRITE   6     // write a file
#define SYS_SIZE    7     // get the size of a file
#define SYS_DELETE  8     // delete a file

#include "interrupt.h"

// This handler is invoked on a system call (ecall).
void syscall_handler(struct trap_frame *);
