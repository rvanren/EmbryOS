#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "string.h"
#include "interrupt.h"

void init_main(), splash_main(), life_main(), snake_main();
static inline void user_exit() {
    register int a7 asm("a7") = SYS_EXIT;
    asm volatile("ecall" : : "r"(a7));
}
void init_crt(){ init_main(); user_exit(); }
void splash_crt(){ splash_main(); user_exit(); }
void life_crt(){ life_main(); user_exit(); }
void snake_crt(){ snake_main(); user_exit(); }
static void (*apps[])() = { init_crt, splash_crt, life_crt, snake_crt };

__attribute__((noreturn)) void enter_user(void (*entry)());

void exec_user(void) {
    struct pcb *self = sched_self();
    enter_user(apps[self->executable - 2]);
}
