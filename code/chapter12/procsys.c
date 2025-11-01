#include <stddef.h>
#include "frame.h"
#include "sched.h"
#include "die.h"
#include "vm.h"
#include "flat.h"
#include "string.h"

void proc_check_legal(struct pcb *self, uintptr_t start, int size) {
    if (size == 0) return;
    uintptr_t end = start + size - 1;
    if (end < start) die("bad system call size");
    if (start < VM_START || end >= VM_END) die("bad system call address");
}

void proc_pagefault(struct trap_frame *tf) {
    extern struct flat flat_fs;
    void *frame = frame_alloc();
    if (frame == 0) die("out of memory");

    struct pcb *self = sched_self();
    uint32_t offset = tf->stval & ~(PAGE_SIZE - 1);
    int n = flat_read(&flat_fs, self->executable, sizeof(uint32_t) + offset - VM_START, frame, PAGE_SIZE);
    if (n > 0) memset((char *) frame + n, 0, PAGE_SIZE - n);
    vm_map(self->base, tf->stval, frame);
}
