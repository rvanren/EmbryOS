#include "trap.h"
#include "syscall.h"
#include "process.h"
#include "stdio.h"
#include "sched.h"
#include "uart.h"

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;

    switch (tf->a7) {
    case SYS_PUT:
        proc_put(self, tf->a0, tf->a1, tf->a2, tf->a3, tf->a4);
        break;

    case SYS_GET:
        tf->a0 = uart_get();  // <-- result returned to user in a0
        break;

    default:
        printf("Unknown syscall %d\n", tf->a7);
    }
}

void syscall_init(void) {
}
