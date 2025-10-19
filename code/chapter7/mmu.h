// mmu.h — EmbryOS minimal Sv32 memory management unit interface
#pragma once
#include <stdint.h>
#include "frame.h"

/* ---- Constants ---- */

// Page and table sizes (Sv32)
#define PTE_PER_PT      1024u     // 4 KiB / 4 B per PTE

// PTE flag bits (RISC-V Sv32)
#define PTE_V   (1u << 0)   // valid
#define PTE_R   (1u << 1)   // readable
#define PTE_W   (1u << 2)   // writable
#define PTE_X   (1u << 3)   // executable
#define PTE_U   (1u << 4)   // user accessible
#define PTE_G   (1u << 5)   // global
#define PTE_A   (1u << 6)   // accessed
#define PTE_D   (1u << 7)   // dirty

// Helper macros
#define PAGE_ALIGN_DOWN(x)  ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x)    (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

/* ---- Types ---- */

// A page table: 1024 entries, 4 bytes each
typedef uint32_t pte_t;
typedef pte_t pagetable_t[PTE_PER_PT];

/* ---- Public API ---- */

// Initialize the kernel’s root page table and map default regions.
// Must be called after frame_init().
void vm_init(void);

// Enable virtual memory by writing satp (Sv32).
// Assumes identity mapping for code currently executing.
void vm_enable(void);

// Return the root page table pointer (physical address).
pagetable_t *vm_root(void);

// Identity map a kernel range: VA == PA, supervisor-only.
void vm_map_kernel_identity(uint32_t start, uint32_t size, uint32_t flags);

// Identity map a user range: VA == PA, user-accessible (U=1).
void vm_map_user_identity(uint32_t start, uint32_t size, uint32_t flags);

// Create a fresh, empty Sv32 root page table (all zeros).
pagetable_t *vm_create_root(void);

// Map a single 4 KiB page: va -> pa with given PTE flags.
int vm_map(pagetable_t *root, uint32_t va, uint32_t pa, uint32_t flags);

// Reproduce the kernel identity mappings into 'dst' (supervisor-only).
// Keeps user region unmapped; you’ll map user pages per-process.
void vm_clone_kernel_mappings(pagetable_t *dst);

// (Optional: centralize these if not already in a shared header)
#define KERN_BASE  0x80000000u
#define KERN_SIZE  0x00400000u
#define USER_BASE  0x80400000u
#define USER_TOP   0x80800000u
