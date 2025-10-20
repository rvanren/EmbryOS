#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "frame.h"
#include "sched.h"

__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val, uintptr_t user_sp, uintptr_t ksp) {
    asm volatile (
        "csrw mscratch, %[ksp]\n"   /* kernel stack for traps */
        "mv   gp, %[gp]\n"
        "mv   sp, %[usp]\n"
        "csrr t0, mstatus\n"
        "li   t1, 0x1800\n"         /* MSTATUS_MPP bits */
        "not  t2, t1\n"
        "and  t0, t0, t2\n"         /* clear MPP */
        "csrw mstatus, t0\n"
        "csrw mepc, %[pc]\n"
        "mret\n"                    /* drop to user mode */
        :
        : [gp]"r"(gp_val),
          [usp]"r"(user_sp),
          [ksp]"r"(ksp),
          [pc]"r"(entry)
        : "t0","t1","t2","gp","memory"
    );
    __builtin_unreachable();
}

void run_user(char start[], char end[], unsigned int gp_offset) {
    struct pcb *self = run_queue[proc_current]->next;
    size_t size = end - start;

    self->base = frame_alloc();
    self->stack = frame_alloc();
    memset(self->base, 0, PAGE_SIZE);
    memset(self->stack, 0, PAGE_SIZE);
    for (size_t i = 0; i < size; i++) self->base[i] = start[i];
    enter_user(self->base, (uintptr_t) (self->base + gp_offset),
                            (uintptr_t) self->stack + PAGE_SIZE,
                            (uintptr_t) self + PAGE_SIZE);
}
