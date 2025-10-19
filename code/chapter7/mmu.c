// mmu.c — EmbryOS minimal Sv32 MMU and page-table management
// Depends on: frame.c (frame_alloc, FRAME)

#include <stdint.h>
#include "frame.h"
#include "mmu.h"

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static inline uint32_t vpn1(uint32_t va) { return (va >> 22) & 0x3FF; }
static inline uint32_t vpn0(uint32_t va) { return (va >> 12) & 0x3FF; }
#define PA2PTE(pa)  (((uint32_t)(pa) >> 2) & ~0x3FFu)

// Global kernel root (used during boot)
static pagetable_t *kernel_root __attribute__((aligned(PAGE_SIZE))) = 0;

// Allocate and clear a new page table (one 4 KiB frame)
static pagetable_t *alloc_pt(void) {
    int f = frame_alloc();
    if (f < 0) return 0;
    pagetable_t *pt = FRAME(pagetable_t, f);
    for (unsigned i = 0; i < PTE_PER_PT; i++) (*pt)[i] = 0;
    return pt;
}

// Walk to the leaf PTE for VA, creating intermediate levels if needed
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

// Map a single 4-KiB page: VA → PA with flags (adds V/A/D bits)
int vm_map(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags) {
    pte_t *pte = walk_create(root, va);
    if (!pte) return -1;
    uint32_t f = flags | PTE_V | PTE_A;   // always mark accessed
    if (flags & PTE_W) f |= PTE_D;        // mark dirty if writable
    *pte = PA2PTE(pa) | f;
    return 0;
}

// Map a range (size multiple of PAGE_SIZE)
static int map_range(pagetable_t *root, uint32_t va, uint32_t pa,
                     uint32_t size, uint32_t flags) {
    for (uint32_t off = 0; off < size; off += PAGE_SIZE)
        if (vm_map(root, va + off, pa + off, flags)) return -1;
    return 0;
}

// ---------------------------------------------------------------------------
// Kernel initialization
// ---------------------------------------------------------------------------

pagetable_t *vm_root(void) { return kernel_root; }

void vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags) {
    if (!kernel_root) return;
    start = PAGE_ALIGN_DOWN(start);
    size  = PAGE_ALIGN_UP(size);
    map_range(kernel_root, start, start, size, flags);
}

void vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags) {
    if (!kernel_root) return;
    start = PAGE_ALIGN_DOWN(start);
    size  = PAGE_ALIGN_UP(size);
    map_range(kernel_root, start, start, size, flags);
}

// Build default kernel + user mappings for boot
void vm_init(void) {
    kernel_root = alloc_pt();
    if (!kernel_root) return;

    // Kernel: 0x80000000–0x80400000, supervisor-only
    vm_map_kernel_identity(KERN_BASE, KERN_SIZE,
                           PTE_R | PTE_W | PTE_X | PTE_G | PTE_A | PTE_D);

    // User: 0x80400000–0x80800000, accessible from U-mode
    vm_map_user_identity(USER_BASE, USER_TOP - USER_BASE,
                         PTE_R | PTE_W | PTE_X | PTE_U | PTE_A | PTE_D);
}

// Enable paging (Sv32)
void vm_enable(void) {
    if (!kernel_root) return;
    uint32_t root_pa = (uint32_t)(uintptr_t)kernel_root;
    uint32_t satp = (1u << 31) | (root_pa >> 12);   // MODE=Sv32
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");
}

// ---------------------------------------------------------------------------
// Per-process page tables
// ---------------------------------------------------------------------------

// Create a new empty root (zeroed)
pagetable_t *vm_create_root(void) {
    return alloc_pt();
}

// Rebuild the kernel identity map inside a new root (shared kernel mapping)
void vm_clone_kernel_mappings(pagetable_t *dst) {
    uint32_t kstart = KERN_BASE;
    uint32_t kend   = KERN_BASE + KERN_SIZE;
    for (uint32_t pa = kstart; pa < kend; pa += PAGE_SIZE)
        vm_map(dst, pa, pa, PTE_R | PTE_W | PTE_X | PTE_G | PTE_A | PTE_D);
}

// ---------------------------------------------------------------------------
// Debug helper (optional)
// ---------------------------------------------------------------------------

#ifdef DEBUG_MMU
#include "stdio.h"
void vm_dump_pte(pagetable_t *root, uint32_t va) {
    uint32_t l1 = vpn1(va), l0 = vpn0(va);
    pte_t pte1 = (*root)[l1];
    printf("L1[%u]=0x%08x\n", l1, pte1);
    if (!(pte1 & PTE_V)) return;
    uint32_t leaf_pa = (pte1 >> 10) << 12;
    pte_t *leaf = (pte_t *)leaf_pa;
    pte_t pte0 = leaf[l0];
    printf("  L0[%u]=0x%08x (U=%d X=%d R=%d W=%d A=%d D=%d V=%d)\n",
           l0, pte0,
           !!(pte0 & PTE_U), !!(pte0 & PTE_X),
           !!(pte0 & PTE_R), !!(pte0 & PTE_W),
           !!(pte0 & PTE_A), !!(pte0 & PTE_D),
           !!(pte0 & PTE_V));
}
#endif
