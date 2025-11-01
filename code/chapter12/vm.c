#include <stddef.h>
#include <stdint.h>
#include "platform.h"
#include "string.h"
#include "frame.h"
#include "process.h"
#include "sched.h"
#include "vm.h"
#include "kprintf.h"

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)

#define PTE_COUNT  (4*1024*1024 / PAGE_SIZE)  // 1024 entries

extern char frames[];    // from linker

static uint32_t root_pt[1024] __attribute__((aligned(PAGE_SIZE)));

void vm_pagefault(struct trap_frame *tf) {
    void *frame = frame_alloc();
    if (frame == 0) kprintf("Out of memory"); for (;;) ;

#ifdef notdef
    // Backing data
    size_t offset = addr - 0x1000;
    uint8_t *dst = FRAME(uint8_t, frame);
    if (offset < p->image_size) {
        size_t n = MIN(PAGE_SIZE, p->image_size - offset);
        memcpy(dst, p->image->data + offset, n);
        if (n < PAGE_SIZE)
            memset(dst + n, 0, PAGE_SIZE - n);
    } else {
        memset(dst, 0, PAGE_SIZE);
    }
#endif
    memset(frame, 0, PAGE_SIZE);

    struct pcb *self = sched_self();
    uint32_t *pt = (uint32_t *) self->base;
    int index = (tf->stval >> 12) & (PTE_COUNT - 1);
    uintptr_t pa = (uintptr_t) frame;
    pt[index] = (pa >> 2) | PTE_V | PTE_R | PTE_W | PTE_X | PTE_U;

    // asm volatile("sfence.vma %0, x0" :: "r"(tf->stval) : "memory");
}

void vm_flush(void) {
    struct pcb *self = sched_self();
    uintptr_t leaf_pt = (uintptr_t) self->base;
    root_pt[0] = (leaf_pt >> 2) | PTE_V;
    asm volatile("sfence.vma" ::: "memory");
}

void vm_init_pt(void *base, void *stack) {
    uint32_t *pt = base;
    memset(pt, 0, FRAME_SIZE - sizeof(*pt));
    uint32_t pa = (uintptr_t) stack;
    pt[1023] = (pa >> 2) | PTE_V | PTE_R | PTE_W | PTE_X | PTE_U;
    vm_flush();
}

void vm_init(void) {
    uint32_t user_start   = (uintptr_t) frames;

    // 4 MB identity mappings for everything below 0x8040_0000
    for (int i = 0; i < 1024; i++) {
        uint32_t pa = i << 22;   // 4 MiB per PTE
        root_pt[i] = (pa >> 2) | PTE_V | PTE_R | PTE_W | PTE_X;
    }

    // ---- Activate ----
    asm volatile("csrs sstatus, %0" :: "r"(1 << 18));  // allow S mode to access U pages
    uint32_t satp = (1u << 31) | (((uint32_t)root_pt) >> 12);
    asm volatile ("csrw satp, %0; sfence.vma" :: "r"(satp));
}
