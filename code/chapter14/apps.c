#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "kprintf.h"
#include "string.h"
#include "interrupt.h"
#include "pmp.h"
#include "flat.h"

extern struct flat flat_fs;

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
    flat_read(&flat_fs, self->executable, 0, &gp_offset, sizeof(gp_offset));
    uint32_t size = flat_size(&flat_fs, self->executable) - sizeof(gp_offset);
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("executable too large<");
        proc_exit();
    }

    // Initialize code/data page
    flat_read(&flat_fs, self->executable, sizeof(gp_offset), self->base, size);

    memset(&self->base[size], 0, PAGE_SIZE - size);
    memset(self->stack, 0, PAGE_SIZE);

    // Initialize stack page
    uintptr_t sp = (uintptr_t) self->stack + PAGE_SIZE;
    sp -= self->size;
    sp &= ~0xF;   // align down to 16 bytes
    memcpy((void *) sp, self->args, self->size);

    pmp_load(self);  // Load PMP registers to isolate app
    enter_user(self->base, (uintptr_t) (self->base + gp_offset), sp, self->size,
                            (uintptr_t) self + PAGE_SIZE);
}
