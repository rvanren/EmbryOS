#ifndef CLINT_H
#define CLINT_H

#include "process.h"

enum { CLINT_TIMER };

void interrupts_enable(void);
void interrupts_disable(void);
int clint_init();
void clint_set_handler(int which, entry_t handler);

#endif // CLINT_H
