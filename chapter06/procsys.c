#include "embryos.h"

// The process had a miss at the given virtual address.
static void proc_miss(struct pcb *self, uintptr_t va) {
    if (vm_is_mapped(self->base, va)) die("proc_miss: already mapped??");

    // Allocate a frame
    void *frame = frame_alloc();
    if (frame == 0) die("out of memory");

    // Initialize it
    uintptr_t offset = va & ~(PAGE_SIZE - 1);

    const struct embedded_file *ef = &embedded_files[self->executable - 2];
    const unsigned char *start = ef->data + PAGE_SIZE + offset - VM_START;
    const unsigned char *end = start + PAGE_SIZE;
    if (end > ef->data + ef->size) end = ef->data + ef->size;
    if (start >= end) memset(frame, 0, PAGE_SIZE);
    else {
        uword_t n = end - start;
        memcpy(frame, start, n);
        if (n < PAGE_SIZE) memset((char *) frame + n, 0, PAGE_SIZE - n);
        icache_flush();
    }

    // Map it
    L3(L_NORM, L_VM_MAP, (uintptr_t) self->base, va, (uintptr_t) frame);
    vm_map(self->base, va, frame);
}

void proc_pagefault(struct trap_frame *tf) {
    L1(L_NORM, L_PAGEFAULT, tf->stval);
    proc_miss(sched_self(), tf->stval);
}
