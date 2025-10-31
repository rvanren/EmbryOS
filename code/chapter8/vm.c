#include <stddef.h>
#include <stdint.h>
#include "vm.h"
#include "kprintf.h"

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

extern char frames[];    // from linker

static uint32_t root_pt[1024] __attribute__((aligned(PAGE_SIZE)));
static uint32_t leaf_pt[1024] __attribute__((aligned(PAGE_SIZE)));

void vm_init(void) {
    uint32_t user_start   = (uintptr_t) frames;

    // ---- 4 MiB identity mappings for everything below 0x8040_0000 ----
    for (int i = 0; i < 1024; i++) {
        uint32_t pa = i << 22;   // 4 MiB per PTE
        if (pa >= 0x80400000 && pa < 0x80800000) continue; // skip special window
        root_pt[i] = (pa >> 2) | PTE_V | PTE_R | PTE_W | PTE_X;
    }

    // ---- Second-level page table for 0x8040_0000 - 0x8080_0000 ----
    for (int i = 0; i < 1024; i++) {
        uint32_t pa = 0x80400000 + i * PAGE_SIZE;
        uint32_t flags = PTE_V | PTE_R | PTE_W | PTE_X;
        // if (pa >= user_start)
    //     flags |= PTE_U;
        leaf_pt[i] = (pa >> 2) | flags;
    }

    // Root entry for 0x8040_0000 - 0x8080_0000
    int idx = 0x80400000 >> 22;
    root_pt[idx] = ((uint32_t)leaf_pt >> 2) | PTE_V;

    // ---- Activate ----
    uint32_t satp = (1u << 31) | (((uint32_t)root_pt) >> 12);
    asm volatile ("csrw satp, %0; sfence.vma" :: "r"(satp));
}
