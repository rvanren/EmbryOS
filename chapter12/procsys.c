#include "embryos.h"

// The process had a miss at the given virtual address.
static void proc_miss(struct pcb *self, uintptr_t va) {
    if (vm_is_mapped(self->base, va)) die("proc_miss: already mapped??");

    // Allocate a frame
    extern struct flat flat_fs;
    void *frame = frame_alloc();
    if (frame == 0) die("out of memory");

    // Initialize it
    uintptr_t offset = va & ~(PAGE_SIZE - 1);
    if (offset < VM_START || offset >= VM_END) die("proc_miss: bad address");
    int n = flat_read(&flat_fs, self->executable,
                    offset - VM_START + PAGE_SIZE, frame, PAGE_SIZE);
    memset((char *) frame + n, 0, PAGE_SIZE - n);
    if (n > 0) icache_flush();

    // Map it
    L3(L_NORM, L_VM_MAP, (uintptr_t) self->base, va, (uintptr_t) frame);
    vm_map(self->base, va, frame);
    self->hart->needs_tlb_flush = 1;
}

void proc_pagefault(struct trap_frame *tf) {
    L1(L_NORM, L_PAGEFAULT, tf->stval);
    proc_miss(sched_self(), tf->stval);
}

void proc_check_legal(struct pcb *self, uintptr_t start, int size) {
    if (size == 0) return;
    uintptr_t end = start + size - 1;
    if (end < start) die("bad system call size");
    if (start < VM_START || end >= VM_END) die("bad system call address");
    uintptr_t va_start = start & ~(PAGE_SIZE - 1);
    uintptr_t va_end   = (start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (uintptr_t va = va_start; va < va_end; va += PAGE_SIZE)
        if (!vm_is_mapped(self->base, va)) proc_miss(self, va);
}
