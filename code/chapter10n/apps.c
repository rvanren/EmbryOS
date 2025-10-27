#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "pmp.h"
#include "stdio.h"
#include "string.h"

#ifdef CH11
#include "flat.h"

extern struct flat flat_fs;
#endif

__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val,
                uintptr_t user_sp, size_t arg_size, uintptr_t ksp);

void exec_user(void) {
    struct pcb *self = run_queue[proc_current]->next;

    self->base = frame_alloc();
    self->stack = frame_alloc();
    if (self->base == 0 || self->stack == 0) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        printf("out of memory<");
        proc_exit();
    }

    uint32_t gp_offset;

#ifdef CH11
    flat_read(&flat_fs, self->executable, 0, &gp_offset, sizeof(gp_offset));
    uint32_t size = flat_size(&flat_fs, self->executable) - sizeof(gp_offset);
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        printf("executable too large<");
        proc_exit();
    }

    // Initialize code/data page
    flat_read(&flat_fs, self->executable, sizeof(gp_offset), self->base, size);
#else
    struct app_info *ai = &app_table[self->executable - 2];
    gp_offset = ai->gp;

    uint32_t size = ai->end - ai->start;
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        printf("executable too large<");
        proc_exit();
    }

    memcpy(self->base, ai->start, size);
#endif

    memset(&self->base[size], 0, PAGE_SIZE - size);
    memset(self->stack, 0, PAGE_SIZE);

    uintptr_t sp = (uintptr_t) self->stack + PAGE_SIZE;
#ifdef CH13
    // Initialize stack page
    sp -= self->size;
    sp &= ~0xF;   // align down to 16 bytes
    memcpy((void *) sp, self->args, self->size);
#endif

    // Load PMP registers
    pmp_load(self);

    enter_user(self->base, (uintptr_t) (self->base + gp_offset), sp, self->size,
                            (uintptr_t) self + PAGE_SIZE);
}
