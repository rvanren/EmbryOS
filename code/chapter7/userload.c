// userload.c — Load and start a user program in EmbryOS

#include <stdint.h>
#include "frame.h"
#include "mmu.h"
#include "process.h"
#include "platform.h"
#include "stdio.h"

// These symbols are created when user.bin is linked into the kernel as user.o
extern uint8_t _binary_user_bin_start[];
extern uint8_t _binary_user_bin_end[];

#define USER_BASE   0x80400000u
#define USER_TOP    0x80800000u
#define USER_SIZE   (USER_TOP - USER_BASE)

/* Copy the user program into its address space */
void user_load(struct pcb *p) {
    uint32_t size = (uint32_t)(_binary_user_bin_end - _binary_user_bin_start);
    if (size > USER_SIZE) {
        printf("User program too large (%u bytes)\n", size);
        return;
    }

    // Copy into the user region (already identity mapped by vm_init)
    uint8_t *dst = (uint8_t *)USER_BASE;
    for (uint32_t i = 0; i < size; i++)
        dst[i] = _binary_user_bin_start[i];

    // Initialize user entry and stack
    p->pc  = USER_BASE;   // entry point
    p->usp = USER_TOP;    // top of user stack
}

/* Enter user mode — assumes vm_enable() has already been called */
void user_run(struct pcb *p) {
    // Select the process's page table (for now, everyone uses kernel_root)
    uint32_t root_pa = (uint32_t)(uintptr_t)p->pagetable;
    uint32_t satp = (1u << 31) | (root_pa >> 12);   // MODE = Sv32
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");

    // Set user entry point
    asm volatile("csrw sepc, %0" :: "r"(p->pc));

    // Enable interrupts and set SPP=0 (next privilege = U)
    uint32_t sstatus;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1 << 8);        // SPP = 0 → U-mode after sret
    sstatus |=  (1 << 5);        // SPIE = 1 → enable interrupts after sret
    asm volatile("csrw sstatus, %0" :: "r"(sstatus));

    // Load user stack pointer (virtual address)
    asm volatile("mv sp, %0" :: "r"(p->usp));

    // Jump into user code
    asm volatile("sret");
}
