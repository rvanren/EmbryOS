#include "trap.h"
#include "syscall.h"
#include "process.h"
#include "process.h"
#include "sched.h"
#include "io.h"
#include "frame.h"
#include "die.h"

void syscall_handler(struct trap_frame *tf) {
    struct pcb *self = run_queue[proc_current]->next;

    switch (tf->a7) {
    case SYS_EXIT:
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        die("process ended");
        break;
    case SYS_SPAWN:
        sched_run(tf->a0, (struct rect){ tf->a1, tf->a2, tf->a3, tf->a4 }, (void *) (uintptr_t) tf->a5, tf->a6);
        break;
    case SYS_PUT:
        proc_put(self, tf->a0, tf->a1, tf->a2);
        break;
    case SYS_GET:
        tf->a0 = io_get(self, tf->a0, tf->a1, tf->a2, tf->a3);
        break;
    default:
        die("unknown system call");
    }
}
