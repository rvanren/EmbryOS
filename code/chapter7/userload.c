// userload.c — load and start an embedded user program
#include <stdint.h>
#include "process.h"
#include "mmu.h"

extern uint8_t _binary_user_bin_start[];
extern uint8_t _binary_user_bin_end[];

#define USER_BASE   0x80400000u
#define USER_TOP    0x80800000u

void load_user(struct pcb *p) {
    uint32_t size = _binary_user_bin_end - _binary_user_bin_start;
    uint8_t *src = _binary_user_bin_start;
    uint8_t *dst = (uint8_t *)USER_BASE;

    // Copy user code into user address space
    for (uint32_t i = 0; i < size; i++)
        dst[i] = src[i];

    // Initialize process state
    p->pc = USER_BASE;
    p->sp = USER_TOP;
}

/* Run a user process by switching to its page table and dropping to U-mode */
void run_user(struct pcb *p) {
    // Switch to process address space
    asm volatile("csrw satp, %0" :: "r"(p->satp));
    asm volatile("sfence.vma zero, zero");

    // Set program counter (entry)
    asm volatile("csrw sepc, %0" :: "r"(p->pc));

    // Prepare stack
    asm volatile("mv sp, %0" :: "r"(p->sp));

    // Enable interrupts in S-mode and set U-mode next
    register uint32_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus &= ~(3 << 11);      // clear MPP
    mstatus |= (1 << 11);       // set MPP = S (so mret → S-mode)
    asm volatile("csrw mstatus, %0" :: "r"(mstatus));

    // mret will enter S-mode, and then sret (in S trap handler) will enter U-mode
    asm volatile("mret");
}
