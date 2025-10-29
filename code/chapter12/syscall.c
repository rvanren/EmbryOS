#include "trap.h"
#include "syscall.h"
#include "process.h"
#include "kprintf.h"
#include "process.h"
#include "sched.h"
#include "io.h"
#include "frame.h"
#include "flat.h"

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;
    extern struct flat flat_fs;

    switch (tf->a7) {
    case SYS_EXIT:
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("process ended<");
        proc_exit();
        break;
    case SYS_SPAWN:
        proc_check_legal(self, (uintptr_t) tf->a5, tf->a6);
        sched_run(tf->a0, (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 }, (void *) (uintptr_t) tf->a5, tf->a6);
        break;
    case SYS_PUT:
        proc_put(self, tf->a0, tf->a1, tf->a2);
        break;
    case SYS_GET:
        tf->a0 = io_get(self, tf->a0, tf->a1, tf->a2, tf->a3);
        break;
    case SYS_CREATE:
        tf->a0 = flat_create(&flat_fs);
        break;
    case SYS_READ:
        proc_check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_read(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_WRITE:
        proc_check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_write(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_SIZE:
        tf->a0 = flat_size(&flat_fs, tf->a0);
        break;
    case SYS_DELETE:
        flat_delete(&flat_fs, tf->a0);
        break;
    default:
        kprintf("Unknown syscall %d\n", tf->a7);
    }
}
