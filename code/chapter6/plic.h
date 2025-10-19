#include "trap.h"

void interrupt_handler(struct trap_frame *tf);
int plic_init();
