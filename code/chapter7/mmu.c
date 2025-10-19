// mmu.c — Minimal Sv32 page tables + tiny page allocator for EmbryOS
// Drop-in file; no external headers required.

#include <stdint.h>
#include <stddef.h>

/* ---------- Sv32 page table basics ---------- */

#define PAGE_SIZE       4096u
#define PTE_PER_PT      1024u     // 4 KiB / 4 B
#define VPN1_SHIFT      22
#define VPN0_SHIFT      12
#define VPN_MASK        0x3FFu
#define PA2PPN(pa)      ((uint32_t)(pa) >> 12)
#define PPN2PTE(ppn)    ((uint32_t)(ppn) << 10)     // goes into bits [31:10]
#define PA2PTE(pa)      PPN2PTE(PA2PPN(pa))         // convenience

/* PTE flags */
#define PTE_V   (1u << 0)   // valid
#define PTE_R   (1u << 1)   // readable
#define PTE_W   (1u << 2)   // writable
#define PTE_X   (1u << 3)   // executable
#define PTE_U   (1u << 4)   // user accessible
#define PTE_G   (1u << 5)   // global (optional)
#define PTE_A   (1u << 6)   // accessed
#define PTE_D   (1u << 7)   // dirty

typedef uint32_t        pte_t;
typedef pte_t           pagetable_t[PTE_PER_PT];

/* ---------- Linker-provided page pool (our simple allocator) ---------- */
/* Your linker script should provide a page-aligned, page-multiple region:
     .frames (NOLOAD) : ALIGN(4096) {
         PROVIDE(frames = .);
         . = . + (4096 * 64);   // e.g., 64 pages
         PROVIDE(__frames_end = .);
     }
*/

extern uint8_t  frames[];        // start of page pool
extern uint8_t  __frames_end[];  // end of page pool (exclusive)

/* Tiny bump allocator over .frames */
static uint8_t *alloc_ptr = NULL;
static uint8_t *alloc_end = NULL;

static void *alloc_page(void) {
    if (!alloc_ptr) {
        alloc_ptr = (uint8_t *)(((uintptr_t)frames + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1));
        alloc_end = (uint8_t *)__frames_end;
    }
    if ((size_t)(alloc_end - alloc_ptr) < PAGE_SIZE) {
        return NULL; // out of pages
    }
    void *p = alloc_ptr;
    alloc_ptr += PAGE_SIZE;

    // zero the page (no libc)
    uint32_t *w = (uint32_t *)p;
    for (size_t i = 0; i < PAGE_SIZE / sizeof(uint32_t); i++) w[i] = 0;

    return p;
}

/* ---------- Kernel page table (root) ---------- */

static pagetable_t *kernel_root __attribute__((aligned(PAGE_SIZE))) = NULL;

/* Extract VPN indices from a virtual address */
static inline uint32_t vpn1(uint32_t va) { return (va >> VPN1_SHIFT) & VPN_MASK; }
static inline uint32_t vpn0(uint32_t va) { return (va >> VPN0_SHIFT) & VPN_MASK; }

/* Resolve/create a second-level page table for a given VA in a root PT */
static pte_t *walk_create(pagetable_t *root, uint32_t va) {
    pte_t *pte1 = &(*root)[vpn1(va)];
    if (!(*pte1 & PTE_V)) {
        pagetable_t *leaf = (pagetable_t *)alloc_page();
        if (!leaf) return NULL;
        // A pointer PTE to a next-level table: V=1, R=W=X=0 (per spec)
        *pte1 = PA2PTE((uintptr_t)leaf) | PTE_V;
    }
    // Recover the second-level table address from pte1
    uintptr_t leaf_pa = (uintptr_t)((*pte1 >> 10) << 12);
    pagetable_t *leaf = (pagetable_t *)leaf_pa;  // kernel is identity-mapped during setup
    return &(*leaf)[vpn0(va)];
}

/* Map a single 4 KiB page: va -> pa with flags */
static int map_page(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags) {
    pte_t *pte0 = walk_create(root, va);
    if (!pte0) return -1;
    *pte0 = PA2PTE(pa) | flags | PTE_V;
    return 0;
}

/* Map a range [pa, pa+len) to virtual addresses starting at va (4 KiB granularity) */
static int map_range(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t len, uint32_t flags) {
    if (len == 0) return 0;
    uint32_t off = 0;
    while (off < len) {
        if (map_page(root, va + off, pa + off, flags) != 0) return -1;
        off += PAGE_SIZE;
    }
    return 0;
}

/* ---------- Public API ---------- */
/* If you’d like a header later, these are the prototypes:
     void     vm_init(void);
     void     vm_enable(void);
     void     vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags);
     void     vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags);
     pagetable_t *vm_root(void);
*/

pagetable_t *vm_root(void) { return kernel_root; }

/* Convenience helpers to map identity ranges */
void vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags) {
    // Typically flags = PTE_R | PTE_W | PTE_X | PTE_G
    if (!kernel_root) return;
    // Align down/up to page boundaries
    uint32_t va = start & ~(PAGE_SIZE - 1);
    uint32_t pa = va;
    uint32_t end = (start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    (void)map_range(kernel_root, va, pa, end - va, flags);
}

void vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags) {
    // Typically flags = PTE_R | PTE_W | PTE_X | PTE_U
    if (!kernel_root) return;
    uint32_t va = start & ~(PAGE_SIZE - 1);
    uint32_t pa = va;
    uint32_t end = (start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    (void)map_range(kernel_root, va, pa, end - va, flags);
}

/* Initialize the kernel root page table and map default regions */
void vm_init(void) {
    kernel_root = (pagetable_t *)alloc_page();
    // Zeroed by alloc_page()

    // --- Default example mappings (adjust to your layout) ---
    // Kernel @ 0x8000_0000 .. 0x8040_0000 identity, no user access
    vm_map_kernel_identity(0x80000000u, 0x00400000u, PTE_R | PTE_W | PTE_X | PTE_G);

    // User demo region @ 0x8040_0000 .. 0x8080_0000 identity, user accessible
    vm_map_user_identity(0x80400000u, 0x00400000u, PTE_R | PTE_W | PTE_X | PTE_U);
}

/* Write satp and enable Sv32. Assumes identity map for code currently executing. */
void vm_enable(void) {
    if (!kernel_root) return;

    // satp (Sv32): [31]=MODE(1), [30:22]=ASID, [21:0]=PPN
    uint32_t root_pa = (uint32_t)(uintptr_t)kernel_root; // identity during setup
    uint32_t satp = (1u << 31) | (PA2PPN(root_pa) & 0x003FFFFFu);

    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");
}

/* Optional: expose flag constants if other units include mmu.c (or mirror in a header) */
#undef  EXPORT_FLAGS
#ifdef   EXPORT_FLAGS
const uint32_t MMU_PTE_V = PTE_V;
const uint32_t MMU_PTE_R = PTE_R;
const uint32_t MMU_PTE_W = PTE_W;
const uint32_t MMU_PTE_X = PTE_X;
const uint32_t MMU_PTE_U = PTE_U;
const uint32_t MMU_PTE_G = PTE_G;
const uint32_t MMU_PTE_A = PTE_A;
const uint32_t MMU_PTE_D = PTE_D;
#endif
