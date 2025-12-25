#pragma once

#include <stdint.h>
#include "platform.h"

// Initialize virtual memory on the given hart.
void vm_init(struct hart *hart);

// Update the page table and flush the TLB.
void vm_flush(struct hart *hart, void *base);

// Map the given virtual address to the given frame.
void vm_map(void *base, uintptr_t va, void *frame);

// See if the given virtual address is mapped.
int vm_is_mapped(void *base, uintptr_t va);

// Release the page table and all the frames that it points to.
void vm_release(void *base);

static inline void icache_flush() { asm volatile("fence.i" ::: "memory"); }
static inline void tlb_flush() { asm volatile("sfence.vma" ::: "memory"); }

static inline void vm_enable(uword_t satp) {
    asm volatile("csrs sstatus, %0" :: "r"((uword_t) 1 << 18)); // SUM
    asm volatile("csrw satp, %0" :: "r"(satp));
}
