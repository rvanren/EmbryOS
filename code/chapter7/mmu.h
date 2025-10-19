// mmu.h — EmbryOS Sv32 memory management unit interface
// Provides kernel and per-process virtual memory utilities.

#pragma once
#include <stdint.h>

// ---------------------------------------------------------------------------
// Sv32 parameters
// ---------------------------------------------------------------------------

#define PAGE_SIZE       4096u
#define PTE_PER_PT      1024u     // 4 KiB / 4 B per PTE

// ---------------------------------------------------------------------------
// PTE flag bits (RISC-V Sv32)
// ---------------------------------------------------------------------------

#define PTE_V   (1u << 0)   // valid
#define PTE_R   (1u << 1)   // readable
#define PTE_W   (1u << 2)   // writable
#define PTE_X   (1u << 3)   // executable
#define PTE_U   (1u << 4)   // user accessible
#define PTE_G   (1u << 5)   // global
#define PTE_A   (1u << 6)   // accessed
#define PTE_D   (1u << 7)   // dirty

// Convenience alignment macros
#define PAGE_ALIGN_DOWN(x)  ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x)    (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// ---------------------------------------------------------------------------
// Virtual address layout (EmbryOS default)
// ---------------------------------------------------------------------------

#define KERN_BASE   0x80000000u
#define KERN_SIZE   0x00400000u   // 4 MiB kernel
#define USER_BASE   0x80400000u
#define USER_TOP    0x80800000u   // 4 MiB user region

// ---------------------------------------------------------------------------
// Page-table data structures
// ---------------------------------------------------------------------------

typedef uint32_t pte_t;
typedef pte_t pagetable_t[PTE_PER_PT];

// ---------------------------------------------------------------------------
// Kernel initialization
// ---------------------------------------------------------------------------

// Build the initial kernel+user mappings and enable paging (Sv32).
void vm_init(void);
void vm_enable(void);

// Return the global kernel root page table (physical address).
pagetable_t *vm_root(void);

// Map kernel/user regions identity (used only during initialization).
void vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags);
void vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags);

// ---------------------------------------------------------------------------
// Per-process virtual memory
// ---------------------------------------------------------------------------

// Create a fresh, empty Sv32 root page table.
pagetable_t *vm_create_root(void);

// Reproduce the kernel identity mappings inside 'dst'.
void vm_clone_kernel_mappings(pagetable_t *dst);

// Map a single 4 KiB page: va -> pa with given flags (adds V/A/D bits).
int vm_map(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags);

// ---------------------------------------------------------------------------
// Optional debugging (compile with -DDEBUG_MMU)
// ---------------------------------------------------------------------------

#ifdef DEBUG_MMU
void vm_dump_pte(pagetable_t *root, uint32_t va);
#endif
