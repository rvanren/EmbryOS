#include "frame.h"
#include "sched.h"
#include "stdio.h"
#include "trap.h"
#include "interrupt.h"
#include "ctx.h"
#include "mtime.h"
#include "syscall.h"
#include "syslib.h"
#include "uart.h"
#include "plic.h"

#define QUANTUM          5000000         // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    self->pagetable = vm_create_root();
    vm_clone_kernel_mappings(self->pagetable);

    // Allocate frames for code and stack, map them
    int f_code = frame_alloc();
    int f_stack = frame_alloc();
    vm_map(self->pagetable, USER_BASE, f_code,
           PTE_R|PTE_X|PTE_U|PTE_A);
    vm_map(self->pagetable, USER_TOP - PAGE_SIZE, f_stack,
           PTE_R|PTE_W|PTE_U|PTE_A|PTE_D);
}

int main(void) {
    frame_init(); intr_init(); plic_init(); syscall_init(); uart_init(); mtime_init();
    vm_init(); vm_enable();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    sched_init(pcb);
    intr_set_handler(INTR_TIMER, timer_handler);
    intr_set_handler(INTR_SYSCALL, syscall_handler);
    intr_set_handler(INTR_EXTERNAL, interrupt_handler);
    mtime_reset(QUANTUM);

    sched_run(taskA, (struct rect){ 0,  0,  40, 12 });  // upper-left
    sched_run(taskA, (struct rect){ 0, 12,  40, 12 });  // lower-left

    sched_idle();
}
