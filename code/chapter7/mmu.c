// mmu.c — Minimal Sv32 paging for EmbryOS
#include <stdint.h>
#include "frame.h"
#include "mmu.h"

// --- add to mmu.c ---

// (Reuses your existing helpers and frame_alloc())
static inline uint32_t vpn1(uint32_t va) { return (va >> 22) & 0x3FF; }
static inline uint32_t vpn0(uint32_t va) { return (va >> 12) & 0x3FF; }
#define PA2PTE(pa)  (((uint32_t)(pa) >> 2) & ~0x3FFu)

static pagetable_t *alloc_pt(void) {
    int f = frame_alloc();
    if (f < 0) return 0;
    pagetable_t *pt = FRAME(pagetable_t, f);
    for (unsigned i = 0; i < PTE_PER_PT; i++) (*pt)[i] = 0;
    return pt;
}

static pte_t *walk_create(pagetable_t *root, uint32_t va) {
    pte_t *pte1 = &(*root)[vpn1(va)];
    if (!(*pte1 & PTE_V)) {
        pagetable_t *leaf = alloc_pt();
        if (!leaf) return 0;
        *pte1 = PA2PTE(leaf) | PTE_V;          // pointer PTE (R=W=X=0)
    }
    uintptr_t leaf_pa = (uintptr_t)((*pte1 >> 10) << 12);
    return &((pagetable_t*)leaf_pa)[0][vpn0(va)];
}

int vm_map(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags) {
    pte_t *pte = walk_create(root, va);
    if (!pte) return -1;
    uint32_t f = flags | PTE_V | PTE_A;
    if (flags & PTE_W) f |= PTE_D;
    *pte = PA2PTE(pa) | f;
    return 0;
}

pagetable_t *vm_create_root(void) {
    return alloc_pt();
}

void vm_clone_kernel_mappings(pagetable_t *dst) {
    // Recreate kernel identity map in 'dst' (supervisor-only).
    // (Same ranges/flags you used in vm_init())
    uint32_t kstart = KERN_BASE;
    uint32_t kend   = KERN_BASE + KERN_SIZE;
    for (uint32_t pa = kstart; pa < kend; pa += PAGE_SIZE)
        vm_map(dst, pa, pa, PTE_R | PTE_W | PTE_X | PTE_G);
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

    // Kernel: 0x80000000-0x80400000, R/W/X for supervisor only
    vm_map_kernel_identity(0x80000000u, 0x00400000u,
                           PTE_R | PTE_W | PTE_X | PTE_G);

    // User region: 0x80400000-0x80800000, accessible from U-mode
    vm_map_user_identity(0x80400000u, 0x00400000u,
                         PTE_R | PTE_W | PTE_X | PTE_U);

    // PLUS: one (or more) stack pages at the top
    vm_map_user_identity(0x80800000u - 0x1000, 0x1000, PTE_R|PTE_W|PTE_U|PTE_A|PTE_D);

}

// Enable Sv32 paging
void vm_enable(void) {
    if (!kernel_root) return;
    uint32_t root_pa = (uint32_t)(uintptr_t)kernel_root;
    uint32_t satp = (1u << 31) | (root_pa >> 12);   // MODE=Sv32
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");
}
