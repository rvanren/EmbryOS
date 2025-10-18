#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_PUT 0  // write a character to the process's window

void syscall_init(void);
void syscall_handler(void);

#endif
