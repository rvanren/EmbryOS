#include "process.h"
#include "sched.h"
#include "kprintf.h"
#include "die.h"

void die(void *msg) {
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("%s<", msg);
    sched_exit();
}
