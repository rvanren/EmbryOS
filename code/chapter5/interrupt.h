#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "process.h"

enum { INTR_TIMER, INTR_EXTERNAL, INTR_SYSCALL, INTR_EXCEPTION };

void intr_enable(void);
void intr_disable(void);
int intr_init();
void intr_set_handler(int which, entry_t handler);

#endif // INTERRUPT_H
