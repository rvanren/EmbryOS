#include <stddef.h>
#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"
#include "syslib.h"
#include "uart.h"
#include "plic.h"
#include "user_gp.h"

#define QUANTUM          50000         // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

__attribute__((noreturn))
static inline void enter_user(uintptr_t mepc, uintptr_t gp, uintptr_t usp, uintptr_t ksp) {
    asm volatile(
        // set per-process kernel stack for trap entry
        "csrw mscratch, %[ksp]\n"
        // set gp and sp for user context
        "mv   gp, %[gp]\n"
        "mv   sp, %[usp]\n"
        // set mepc to user entry
        "csrw mepc, %[pc]\n"
        // set MPP = U so mret goes to user mode
        "csrr t0, mstatus\n"
        "li   t1, 0x1800        \n"   // MSTATUS_MPP mask (bits 12..11)
        "andn t0, t0, t1        \n"   // clear MPP
        "csrw mstatus, t0       \n"
        "mret\n"
        :
        : [ksp]"r"(ksp), [gp]"r"(gp), [usp]"r"(usp), [pc]"r"(mepc)
        : "t0","t1","gp","sp","memory"
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

int main(void) {
    frame_init(); intr_init(); plic_init(); uart_init(); mtime_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    mtime_reset(QUANTUM);
    ctx_user_setup();

    sched_run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left

    sched_idle();
}
