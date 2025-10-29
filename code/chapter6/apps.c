#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "string.h"
#include "interrupt.h"
#include "die.h"

void init_main(), splash_main(), life_main(), snake_main();
static void (*apps[])() = { init_main, splash_main, life_main, snake_main };

void exec_user(void) {
    struct pcb *self = run_queue[proc_current]->next;
    intr_enable();
    apps[self->executable - 2]();
    intr_disable();
    die("process ended");
}
