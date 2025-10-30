#include <stdint.h>
#include "frame.h"
#include "vm.h"

#define PGSIZE 4096
#define NPTENTRIES 1024

// PTE flags
#define PTE_V 0x001
#define PTE_R 0x002
#define PTE_W 0x004
#define PTE_X 0x008
#define PTE_U 0x010

#define SATP_MODE_SV32 (1u << 31)

static uint32_t *root_pt;
static uint32_t *leaf_kernel;
static uint32_t *leaf_user;

void vm_init(void) {
    // --- allocate page tables ---
    root_pt     = frame_alloc();   // top-level page table
    leaf_kernel = frame_alloc();   // kernel mapping (VPN[1] = 512)
    leaf_user   = frame_alloc();   // user mapping (VPN[1] = 0)

    // --- link kernel leaf ---
    uint32_t vpn1_k = (0x80000000 >> 22) & 0x3FF;   // =512
    root_pt[vpn1_k] = ((uint32_t)leaf_kernel >> 2) | PTE_V;

    // identity-map 4 MB region for kernel
    for (int i = 0; i < 1024; i++) {
        uint32_t pa = 0x80000000 + i * PGSIZE;
        leaf_kernel[i] = ((pa >> 12) << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
    }

    // --- link user leaf ---
    uint32_t vpn1_u = 0;
    root_pt[vpn1_u] = ((uint32_t)leaf_user >> 2) | PTE_V;

    // map only one user page at virtual 0x1000
    int page_index = 1;                      // VPN[0] = 1 → 0x1000–0x1FFF
    void *frame = frame_alloc();             // physical page
    uint32_t pa = (uint32_t)frame;
    leaf_user[page_index] = ((pa >> 12) << 10)
                          | PTE_V | PTE_R | PTE_W | PTE_X | PTE_U;

    // --- enable MMU ---
    uint32_t satp = SATP_MODE_SV32 | ((uint32_t)root_pt >> 12);
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma");
}
