#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "pmp.h"
#include "stdio.h"

__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val, uintptr_t user_sp, uintptr_t ksp);

void run_user(char start[], char end[], unsigned int gp_offset) {
    struct pcb *self = run_queue[proc_current]->next;
    size_t size = end - start;

    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, '>', 0, 1);
        printf("run_user: executable too large<");
        proc_exit();
    }

    self->base = frame_alloc();
    self->stack = frame_alloc();
    for (size_t i = 0; i < size; i++) self->base[i] = start[i];
    for (size_t i = size; i < PAGE_SIZE; i++) self->base[i] = 0;
    for (size_t i = 0; i < PAGE_SIZE; i++) self->stack[i] = 0;

    pmp_config(self);
    pmp_load(self);

    enter_user(self->base, (uintptr_t) (self->base + gp_offset),
                            (uintptr_t) self->stack + PAGE_SIZE,
                            (uintptr_t) self + PAGE_SIZE);
}
