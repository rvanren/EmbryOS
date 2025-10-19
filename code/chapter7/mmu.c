// mmu.c — Minimal Sv32 paging for EmbryOS
// Depends on: frame_alloc() from frame.c

#include <stdint.h>
#include "frame.h"
#include "mmu.h"

// ---- Internal helpers ----

// Extract VPN indices from a virtual address
static inline uint32_t vpn1(uint32_t va) { return (va >> 22) & 0x3FF; }
static inline uint32_t vpn0(uint32_t va) { return (va >> 12) & 0x3FF; }

// Convert a physical address to PTE format
#define PA2PTE(pa)  (((uint32_t)(pa) >> 2) & ~0x3FFu)

// Global kernel root page table
static pagetable_t *kernel_root __attribute__((aligned(PAGE_SIZE))) = 0;

// Allocate and clear a new page table
static pagetable_t *alloc_pt(void) {
    int f = frame_alloc();
    if (f < 0) return 0;
    pagetable_t *pt = FRAME(pagetable_t, f);
    for (unsigned i = 0; i < PTE_PER_PT; i++) (*pt)[i] = 0;
    return pt;
}

// Walk or create the next-level page table
static pte_t *walk_create(pagetable_t *root, uint32_t va) {
    pte_t *pte1 = &(*root)[vpn1(va)];
    if (!(*pte1 & PTE_V)) {
        pagetable_t *leaf = alloc_pt();
        if (!leaf) return 0;
        *pte1 = PA2PTE(leaf) | PTE_V;
    }
    uintptr_t leaf_pa = (uintptr_t)((*pte1 >> 10) << 12);
    pagetable_t *leaf = (pagetable_t *)leaf_pa;   // identity during setup
    return &(*leaf)[vpn0(va)];
}

// Map one 4 KiB page
static int map_page(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags) {
    pte_t *pte = walk_create(root, va);
    if (!pte) return -1;
    *pte = PA2PTE(pa) | flags | PTE_V;
    return 0;
}

// Map a contiguous range (size multiple of PAGE_SIZE)
static int map_range(pagetable_t *root, uint32_t va, uint32_t pa,
                     uint32_t size, uint32_t flags) {
    for (uint32_t off = 0; off < size; off += PAGE_SIZE)
        if (map_page(root, va + off, pa + off, flags)) return -1;
    return 0;
}

// ---- Public interface ----

pagetable_t *vm_root(void) { return kernel_root; }

// Kernel identity map
void vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags) {
    if (!kernel_root) return;
    start = PAGE_ALIGN_DOWN(start);
    size  = PAGE_ALIGN_UP(size);
    map_range(kernel_root, start, start, size, flags);
}

// User identity map
void vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags) {
    if (!kernel_root) return;
    start = PAGE_ALIGN_DOWN(start);
    size  = PAGE_ALIGN_UP(size);
    map_range(kernel_root, start, start, size, flags);
}

// Build default kernel + user mappings
void vm_init(void) {
    kernel_root = alloc_pt();
    if (!kernel_root) return;

    // Kernel: 0x80000000–0x80400000, R/W/X for supervisor only
    vm_map_kernel_identity(0x80000000u, 0x00400000u,
                           PTE_R | PTE_W | PTE_X | PTE_G);

    // User region: 0x80400000–0x80800000, accessible from U-mode
    vm_map_user_identity(0x80400000u, 0x00400000u,
                         PTE_R | PTE_W | PTE_X | PTE_U);
}

// Enable Sv32 paging
void vm_enable(void) {
    if (!kernel_root) return;
    uint32_t root_pa = (uint32_t)(uintptr_t)kernel_root;
    uint32_t satp = (1u << 31) | (root_pa >> 12);   // MODE=Sv32
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");
}
