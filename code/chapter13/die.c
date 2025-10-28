#include "process.h"
#include "kprintf.h"
#include "die.h"

void die(void *msg) {
    struct pcb *self = run_queue[proc_current]->next;
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("%s<", msg);
    proc_exit();
}
