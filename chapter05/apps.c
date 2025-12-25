#include "embryos.h"

#define N_APPS (sizeof(apps) / sizeof(apps[0]))

extern void enter_user(void (*f)(void));

void init_main(), splash_main(), life_main(), snake_main(), pong_main();
static void (*apps[])() = { init_main, splash_main, life_main, snake_main, pong_main };

// This function is invoked from ctx_start() in a new process
void exec_user(void) {
    struct pcb *self = sched_self();
    if (self->executable < 2 || self->executable - 2 >= N_APPS) die("bad executable");
    enter_user(apps[sched_self()->executable - 2]);
    die("process ended");
}
