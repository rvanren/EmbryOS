#include "embryos.h"

__attribute__((noreturn))
void enter_user(uintptr_t entry, uintptr_t user_sp, uintptr_t kernel_sp);

// This function is invoked from ctx_start() in a new process
void exec_user(void) {
    struct pcb *self = sched_self();

    // Count the arguments and compute the total size of the strings.
    int nargs = 0, size = 0, next = 0;
    for (int i = 0; i < self->size; i++)
        if (self->args[i] == 0) {
            nargs++;
            size += (((i + 1) - next) + 7) & ~7; // multiple of 8
            next = i + 1;
        }
    int total = ((size + (nargs + 2) * sizeof(uword_t)) + 0xF) & ~0xF;
    if (total > PAGE_SIZE) die("too many arguments");

    self->base = frame_alloc();     // allocate page table
    char *stack = frame_alloc();    // allocate stack
    if (self->base == 0 || stack == 0) die("out of memory");
    memset(self->base, 0, PAGE_SIZE);

    // initialize the stack with argc, argv, and the arguments themselves
    memset(stack, 0, PAGE_SIZE);
    uword_t *argv = (uword_t *) (stack + PAGE_SIZE - total);
    char *ptop = (char *) (stack + PAGE_SIZE - size);
    char *vtop = (char *) ((uintptr_t) VM_END - size);
    argv[0] = nargs;
    nargs = 1;

    // copy over the arguments
    next = 0;
    for (int i = 0; i < self->size; i++)
        if (self->args[i] == 0) {
            strcpy(ptop, &self->args[next]);
            argv[nargs++] = (uword_t) vtop;
            ptop += (((i + 1) - next) + 7) & ~7; // multiple of 8
            vtop += (((i + 1) - next) + 7) & ~7; // multiple of 8
            next = i + 1;
        }
    argv[nargs] = 0;
    if (vtop != (char *) VM_END) die("PROBLEM");

    // user space runs without the Big Kernel Lock.
    // It is re-acquired by the interrupt handler.
    extern spinlock_t bkl;
    spinlock_release(&bkl);

    // enter user mode
    vm_map(self->base, VM_END - PAGE_SIZE, stack);
    vm_flush(self->hart, self->base);
    enter_user(VM_START, VM_END - total,
            (uintptr_t) self + PAGE_SIZE - TRAP_FRAME_SIZE);
}
