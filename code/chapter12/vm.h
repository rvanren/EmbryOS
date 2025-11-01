#include "trap.h"

void vm_init(void);
void vm_pagefault(struct trap_frame *tf);
