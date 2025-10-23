#include "trap.h"
#include "syscall.h"
#include "process.h"
#include "stdio.h"
#include "process.h"
#include "sched.h"
#include "uart.h"
#include "flat.h"

void check_legal(struct pcb *p, uintptr_t start, int size) {
    uintptr_t end = start + size - 1;
    if (end < start) return 0;
    uintptr_t base_lo  = (uintptr_t) p->base, base_hi  = base_lo + PAGE_SIZE - 1;
    uintptr_t stack_lo = (uintptr_t) p->stack, stack_hi = stack_lo + PAGE_SIZE - 1;
    if (!((start >= base_lo  && end <= base_hi) || (start >= stack_lo && end <= stack_hi))) {
        proc_put(self, 0, 0, '>', 0, 1);
        printf("bad system call address<");
        proc_exit();
    }
}

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;
    extern struct flat flat_fs;
    extern void exec_user();

    switch (tf->a7) {
    case SYS_EXIT:
        proc_put(self, 0, 0, '>', 0, 1);
        printf("process ended<");
        proc_exit();
        break;
    case SYS_SPAWN:
        check_legal(self, (uintptr_t) tf->a5, tf->a6);
        sched_run(tf->a0, (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 }, (void *) (uintptr_t) tf->a5, tf->a6, exec_user);
        break;
    case SYS_PUT:
        proc_put(self, tf->a0, tf->a1, tf->a2, tf->a3, tf->a4);
        break;
    case SYS_GET:
        tf->a0 = uart_get();
        break;

    case SYS_CREATE:
        tf->a0 = flat_create(&flat_fs);
        break;
    case SYS_READ:
        check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_read(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_WRITE:
        check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_write(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_SIZE:
        tf->a0 = flat_size(&flat_fs, tf->a0);
        break;
    case SYS_DELETE:
        flat_delete(&flat_fs, tf->a0);
        break;

    default:
        printf("Unknown syscall %d\n", tf->a7);
    }
}
