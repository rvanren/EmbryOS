#include "process.h"
#include "clint.h"

void interrupts_enable(void);
void interrupts_disable(void);
int clint_init();
void clint_set_handler(int which, entry_t handler);
