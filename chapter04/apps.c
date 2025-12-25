#include "embryos.h"

extern void enter_user(void (*f)(void));

void init_main(), splash_main(), life_main();
static void (*apps[])() = { init_main, splash_main, life_main };

// This function is invoked from ctx_start() in a new process
void exec_user(void) {
    enter_user(apps[sched_self()->executable - 2]);
    die("process ended");
}
