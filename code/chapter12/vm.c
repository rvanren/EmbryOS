#include <stddef.h>
#include <stdint.h>
#include "platform.h"
#include "vm.h"

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)

extern char frames[];    // from linker

static uint32_t root_pt[1024] __attribute__((aligned(PAGE_SIZE)));

void vm_pagefault(struct trap_frame *tf) {
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
