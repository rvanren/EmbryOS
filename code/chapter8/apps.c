#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "trap.h"

#define MSTATUS_MPP_MASK   (3UL << 11)

#define read_csr(reg) ({ unsigned long __tmp; \
    asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp; })

#define write_csr(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#ifdef notdef
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
#endif


void enter_user(void *entry, uintptr_t gp_val, uintptr_t user_sp, uintptr_t ksp) {
    extern void to_user(struct trap_frame *tf);
    struct trap_frame tf = {0};       // zero all fields

    tf.gp      = gp_val;
    tf.sp      = user_sp;
    tf.mepc    = (uintptr_t)entry;
    tf.mstatus = read_csr(mstatus) & ~MSTATUS_MPP_MASK;  // set MPP=U
    write_csr(mscratch, ksp);         // kernel stack for traps

    to_user(&tf);            // never returns
}

void run_user(char start[], char end[], unsigned int gp_offset) {
    struct pcb *self = run_queue[proc_current]->next;
    size_t size = end - start;

    if (size > PAGE_SIZE) {
        printf("run_user: executable too large\n");
        proc_exit();
    }

    self->base = frame_alloc();
    self->stack = frame_alloc();
    for (size_t i = 0; i < size; i++) self->base[i] = start[i];
    for (size_t i = size; i < PAGE_SIZE; i++) self->base[i] = 0;
    for (size_t i = 0; i < PAGE_SIZE; i++) self->stack[i] = 0;
    enter_user(self->base, (uintptr_t) (self->base + gp_offset),
                            (uintptr_t) self->stack + PAGE_SIZE,
                            (uintptr_t) self + PAGE_SIZE);
}
