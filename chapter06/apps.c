#include "embryos.h"

__attribute__((noreturn))
void enter_user(uintptr_t entry, uintptr_t user_sp, uintptr_t kernel_sp);

// This function is invoked from ctx_start() in a new process
void exec_user(void) {
    struct pcb *self = sched_self();

    self->base = frame_alloc();     // allocate page table
    if (self->base == 0) die("out of memory");
    memset(self->base, 0, PAGE_SIZE);

    // enter user mode
    vm_flush(self->hart, self->base);
    enter_user(VM_START, VM_END,
            (uintptr_t) self + PAGE_SIZE - TRAP_FRAME_SIZE);
}
