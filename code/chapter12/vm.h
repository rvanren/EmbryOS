#include "trap.h"
#include "platform.h"

#define VM_START        PAGE_SIZE
#define VM_END          0x400000        // 4 MB

void vm_init(void);
void vm_init_pt(void *base, void *stack);
void vm_flush();
void vm_map(void *base, uintptr_t addr, void *frame);
void vm_release(void * base);
