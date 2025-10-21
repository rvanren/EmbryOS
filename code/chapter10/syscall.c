#include "trap.h"
#include "syscall.h"
#include "process.h"
#include "stdio.h"
#include "process.h"
#include "sched.h"
#include "uart.h"

extern void (*applications[])();
extern int n_applications;

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;

    switch (tf->a7) {
    case SYS_EXIT:
        proc_put(self, 0, 0, '>', 0, 1);
        printf("process ended");
        proc_exit();
        break;
    case SYS_SPAWN:
        if (tf->a0 >= 0 && tf->a0 < n_applications)
            sched_run(applications[tf->a0],
                (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 });
        break;
    case SYS_PUT:
        proc_put(self, tf->a0, tf->a1, tf->a2, tf->a3, tf->a4);
        break;
    case SYS_GET:
        tf->a0 = uart_get();
        break;
    default:
        printf("Unknown syscall %d\n", tf->a7);
    }
}
