#include "embryos.h"

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();

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
        sched_run(tf->a0, (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 },
                            (void *) (uintptr_t) tf->a5, tf->a6);
        break;
    case SYS_PUT:
        L3(L_FREQ, L_USER_PUT, tf->a0, tf->a1, tf->a2);
        proc_put(self, tf->a0, tf->a1, tf->a2);
        break;
    case SYS_GET:
        L1(L_NORM, L_USER_GET, tf->a0);
        tf->a0 = io_get(self, tf->a0);
        break;
    default:
        die("unknown system call");
    }
    tf->sepc += 4;                      // skip ecall
}
