#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "user_gp.h"

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

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    extern char _binary_user_bin_start[], _binary_user_bin_end[];
    size_t size = _binary_user_bin_end - _binary_user_bin_start;
    char *base = frame_alloc(), *stack = frame_alloc();
    for (int i = 0; i < size; i++) base[i] = _binary_user_bin_start[i];
    enter_user(base, (uintptr_t) (base + USER_GP_OFFSET),
                            (uintptr_t) stack + PAGE_SIZE,
                            (uintptr_t) self + PAGE_SIZE);
}

void (*applications[])() = { taskA };
int n_applications = sizeof(applications) / sizeof(applications[0]);

void apps_init(){
    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
}
