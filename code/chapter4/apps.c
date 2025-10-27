#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "kprintf.h"
#include "string.h"
#include "interrupt.h"

void init_main(), splash_main(), life_main();
static void (*apps[])() = { init_main, splash_main, life_main };

void exec_user(void) {
    struct pcb *self = run_queue[proc_current]->next;
    intr_enable();
    apps[self->executable - 2]();
    intr_disable();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("process ended<");
    proc_exit();
}
