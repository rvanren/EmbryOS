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

    // Get the gp_offset
    uint32_t gp_offset;
    flat_read(&flat_fs, self->executable, 0, &gp_offset, sizeof(gp_offset));

    // Initialize the stack
    memset(self->stack, 0, FRAME_SIZE);
    int args_size = (self->size + 0xF) & ~0xF;    // round up to 16 bytes
    uintptr_t sp = (uintptr_t) self->stack + FRAME_SIZE - args_size;
    memcpy((void *) sp, self->args, self->size);

    vm_init_pt(self->base, self->stack);

    enter_user((void *) FRAME_SIZE, (uintptr_t) (FRAME_SIZE + gp_offset),
            0x400000 - args_size, self->size, (uintptr_t) self + FRAME_SIZE);
}
