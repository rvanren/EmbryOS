#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "process.h"
#include "trap.h"

typedef void (*trap_entry_t)(struct trap_frame *);

enum { INTR_TIMER, INTR_EXTERNAL, INTR_SYSCALL, INTR_EXCEPTION };

void intr_enable(void);
void intr_disable(void);
int intr_init();
void intr_set_handler(int which, trap_entry_t handler);

void interrupt_handler(struct trap_frame *);

#endif // INTERRUPT_H
