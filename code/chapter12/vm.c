#include <stddef.h>
#include <stdint.h>
#include "platform.h"
#include "string.h"
#include "frame.h"
#include "vm.h"
#include "die.h"

enum { V, R, W, X, U };         // bits in PTE
#define PTE(x) (1 << (x))
#define PTE_COUNT  (4*1024*1024 / PAGE_SIZE)  // 1024 entries

static uint32_t root_pt[1024] __attribute__((aligned(PAGE_SIZE)));

void vm_map(void *base, uintptr_t addr, void *frame) {
    uint32_t *pt = (uint32_t *) base;
    int index = (addr >> 12) & (PTE_COUNT - 1);
    uintptr_t pa = (uintptr_t) frame;
    pt[index] = ((pa & ~0xFFF) >> 2) | PTE(V) | PTE(R) | PTE(W) | PTE(X) | PTE(U);
}

void vm_flush(void *base) {
    uintptr_t leaf_pt = (uintptr_t) base;
    root_pt[0] = (leaf_pt & ~0xFFF) >> 2 | PTE(V);
    asm volatile("sfence.vma" ::: "memory");
}

void vm_init_pt(void *base, void *stack) {
    uint32_t *pt = base;
    memset(pt, 0, PAGE_SIZE);
    uintptr_t pa = (uintptr_t) stack;
    pt[PTE_COUNT - 1] = ((pa >> 12) << 10) | (PTE(V) | PTE(R) | PTE(W) | PTE(U));
    vm_flush(base);
}

void vm_release(void *base) {
    uint32_t *pt = (uint32_t *) base;

    for (int i = 0; i < PTE_COUNT; i++) {
        uint32_t pte = pt[i];
        if (pte & PTE(V))
            frame_release((void *) ((uintptr_t)(pte >> 10) << 12));
    }
}

void vm_init(void) {
    for (int i = 0; i < MEM_END / (1 << 22); i++) {
        uint32_t pa = i << 22;   // 4 MiB per PTE
        root_pt[i] = (pa >> 2) | PTE(V) | PTE(R) | PTE(W) | PTE(X);
    }
    asm volatile("csrs sstatus, %0" :: "r"(1 << 18));  // allow S mode to access U pages
    uint32_t satp = (1u << 31) | (((uint32_t)root_pt) >> 12);
    asm volatile ("csrw satp, %0; sfence.vma" :: "r"(satp));
}
