#include "trap.h"

void vm_init(void);
void vm_init_pt(void *base, void *stack);
void vm_pagefault(struct trap_frame *tf);
