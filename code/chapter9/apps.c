#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "kprintf.h"
#include "string.h"
#include "interrupt.h"

#include "app_info.h"
__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val,
                uintptr_t user_sp, size_t arg_size, uintptr_t ksp);

void exec_user(void) {
    struct pcb *self = run_queue[proc_current]->next;

    self->base = frame_alloc();
    self->stack = frame_alloc();
    if (self->base == 0 || self->stack == 0) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("out of memory<");
        proc_exit();
    }

    uint32_t gp_offset;
    struct app_info *ai = &app_table[self->executable - 2];
    gp_offset = ai->gp;

    uint32_t size = ai->end - ai->start;
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("executable too large<");
        proc_exit();
    }

    memcpy(self->base, ai->start, size);
    memset(&self->base[size], 0, PAGE_SIZE - size);
    memset(self->stack, 0, PAGE_SIZE);

    uintptr_t sp = (uintptr_t) self->stack + PAGE_SIZE;
    pmp_load(self); // Load PMP registers
    enter_user(self->base, (uintptr_t) (self->base + gp_offset), sp, self->size,
                            (uintptr_t) self + PAGE_SIZE);
}
