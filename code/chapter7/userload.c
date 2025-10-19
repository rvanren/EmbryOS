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

static void dump_pte(uint32_t va, pagetable_t *root) {
    uint32_t l1i = (va >> 22) & 0x3FF;
    uint32_t l0i = (va >> 12) & 0x3FF;

    pte_t pte1 = (*root)[l1i];
    printf("L1[%u]=0x%x\n", l1i, pte1);
    if ((pte1 & PTE_V) == 0) { printf("L1 invalid\n"); return; }

    uint32_t leaf_pa = (pte1 >> 10) << 12;       // physical address of L0
    pte_t *leaf = (pte_t *)leaf_pa;
    pte_t pte0 = leaf[l0i];
    printf("  L0[%u]=0x%x (U=%d X=%d R=%d W=%d V=%d)\n", l0i, pte0,
           !!(pte0 & PTE_U), !!(pte0 & PTE_X),
           !!(pte0 & PTE_R), !!(pte0 & PTE_W), !!(pte0 & PTE_V));
}

/* Enter user mode - assumes vm_enable() has already been called */
void user_run(struct pcb *p) {
    // Set user entry point
    asm volatile("csrw mepc, %0" :: "r"(p->pc));

    // Enable interrupts and set MPP = U
    uint32_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3 << 11);   // clear MPP bits
    mstatus |=  (0 << 11);   // MPP = 0 → U-mode
    mstatus |=  (1 << 7);    // MPIE = 1
    asm volatile("csrw mstatus, %0" :: "r"(mstatus));

    // Switch page tables if desired (Sv32 works in M-mode too)
    uint32_t satp = (1u << 31) | ((uint32_t)p->pagetable >> 12);
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero");

    // Set up user stack
    asm volatile("mv sp, %0" :: "r"(p->usp));

    uint32_t Xsatp, Xmstatus, Xmepc;
    asm volatile("csrr %0, satp"    : "=r"(Xsatp));
    asm volatile("csrr %0, mstatus" : "=r"(Xmstatus));
    asm volatile("csrr %0, mepc"    : "=r"(Xmepc));
    printf("satp=0x%x mstatus=0x%x mepc=0x%x\n", Xsatp, Xmstatus, Xmepc);
    dump_pte(0x80400000u, vm_root());
    printf("user[0]=0x%x\n", *(volatile uint32_t*)0x80400000);
    // Jump to user mode
    asm volatile("mret");
}
