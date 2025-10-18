#ifndef CLINT_H
#define CLINT_H

#include "process.h"

#define CLINT_BASE       0x02000000UL

enum { CLINT_TIMER, CLINT_INTERRUPT, CLINT_SYSCALL, CLINT_EXCEPTION };

void interrupts_enable(void);
void interrupts_disable(void);
int clint_init();
void clint_set_handler(int which, entry_t handler);

#endif // CLINT_H
