#include <stdint.h>

#define PAGESIZE  4096
#define PTE_V     (1 << 0)
#define PTE_R     (1 << 1)
#define PTE_W     (1 << 2)
#define PTE_X     (1 << 3)
#define PTE_U     (1 << 4)
#define SATP_MODE_SV32  (1u << 31)

static uint32_t root_page_table[1024] __attribute__((aligned(PAGESIZE)));

void vm_init(void) {
    // Map the full 4 GB 1:1 with large (4 MB) leaf entries.
    for (int i = 0; i < 1024; i++) {
        uint32_t pa = i << 22; // 4 MB per entry
        // Supervisor leaf PTE: readable, writable, executable, valid.
        root_page_table[i] = (pa >> 2) | (PTE_V | PTE_R | PTE_W | PTE_X);
    }

    // Load satp
    uint32_t root_ppn = ((uint32_t)root_page_table) >> 12;
    uint32_t satp = SATP_MODE_SV32 | root_ppn;
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma");
}
