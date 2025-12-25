#include "embryos.h"

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    extern struct flat flat_fs;

    switch (tf->a7) {
    case SYS_EXIT:
        L0(L_NORM, L_USER_EXIT);
        die("process ended");
        break;
    case SYS_YIELD:
        L0(L_NORM, L_USER_YIELD);
        break;
    case SYS_SPAWN:
        L1(L_NORM, L_USER_SPAWN, tf->a0);
        proc_check_legal(self, (uintptr_t) tf->a5, tf->a6);
        if (tf->a6 > PAGE_SIZE) die("too many arguments");
        sched_run(tf->a0, (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 }, (void *) (uintptr_t) tf->a5, tf->a6);
        break;
    case SYS_PUT:
        L3(L_FREQ, L_USER_PUT, tf->a0, tf->a1, tf->a2);
        proc_put(self, tf->a0, tf->a1, tf->a2);
        break;
    case SYS_GET:
        L1(L_NORM, L_USER_GET, tf->a0);
        tf->a0 = io_get(self, tf->a0);
        break;
    case SYS_CREATE:
        L0(L_NORM, L_USER_CREATE);
        tf->a0 = flat_create(&flat_fs);
        break;
    case SYS_READ:
        L4(L_NORM, L_USER_READ, tf->a0, tf->a1, tf->a2, tf->a3);
        proc_check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_read(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_WRITE:
        L4(L_NORM, L_USER_WRITE, tf->a0, tf->a1, tf->a2, tf->a3);
        proc_check_legal(self, (uintptr_t) tf->a2, tf->a3);
        tf->a0 = flat_write(&flat_fs, tf->a0, tf->a1, (void *) (uintptr_t) tf->a2, tf->a3);
        break;
    case SYS_SIZE:
        L1(L_NORM, L_USER_SIZE, tf->a0);
        tf->a0 = flat_size(&flat_fs, tf->a0);
        break;
    case SYS_DELETE:
        L1(L_NORM, L_USER_DELETE, tf->a0);
        flat_delete(&flat_fs, tf->a0);
        break;
    default:
        die("unknown system call");
    }
    tf->sepc += 4;                      // skip ecall
}
