#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "string.h"
#include "interrupt.h"
#include "vm.h"
#include "flat.h"
#include "die.h"

extern struct flat flat_fs;

#include "app_info.h"
__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val,
                uintptr_t user_sp, size_t arg_size, uintptr_t ksp);

void exec_user(void) {
    struct pcb *self = sched_self();

    self->base = frame_alloc();
    self->stack = frame_alloc();
    if (self->base == 0 || self->stack == 0) die("out of memory");

    uint32_t gp_offset;
    flat_read(&flat_fs, self->executable, 0, &gp_offset, sizeof(gp_offset));
    uint32_t size = flat_size(&flat_fs, self->executable) - sizeof(gp_offset);
    if (size > FRAME_SIZE) die("executable too large");

    // Initialize code/data page
    flat_read(&flat_fs, self->executable, sizeof(gp_offset), self->base, size);

    memset(&self->base[size], 0, FRAME_SIZE - size);
    memset(self->stack, 0, FRAME_SIZE);

    uintptr_t sp = (uintptr_t) self->stack + FRAME_SIZE;
    enter_user(self->base, (uintptr_t) (self->base + gp_offset), sp, self->size,
                            (uintptr_t) self + FRAME_SIZE);
}
