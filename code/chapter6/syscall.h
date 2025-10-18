#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_PUT 0
#define SYS_GET 1

void syscall_init(void);
void syscall_handler(void);

#endif
