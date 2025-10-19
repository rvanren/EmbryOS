#include "frame.h"                                                              #include "sched.h"                                                              #include "stdio.h"                                                              #include "trap.h"                                                               #include "interrupt.h"                                                          #include "ctx.h"                                                                #include "mtime.h"                                                              #include "syscall.h"                                                            #include "syslib.h"
#include "uart.h"
#include "plic.h"

#define QUANTUM          5000000         // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    mtime_reset(QUANTUM); // add another quantum
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    void user_load(struct pcb *p);
    void user_run(struct pcb *p);
    self->pagetable = vm_root();
    printf("LOAD\n");
    user_load(self);
    printf("RUN\n");
    user_run(self);
    printf("DONE\n");
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
