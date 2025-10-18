#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_PUT 0
#define SYS_GET 1

#include "interrupt.h"

void syscall_init(void);
void syscall_handler(struct trap_frame *);

#endif
