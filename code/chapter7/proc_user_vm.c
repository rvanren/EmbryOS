// proc_user_vm.c — per-process address spaces for EmbryOS
#include <stdint.h>
#include "frame.h"
#include "mmu.h"
#include "process.h"
#include "stdio.h"

// Embedded user binary (from user.o)
extern uint8_t _binary_user_bin_start[];
extern uint8_t _binary_user_bin_end[];

static inline uint32_t pg_align_down(uint32_t x) { return x & ~(PAGE_SIZE-1); }

// Build a private root PT for a process: kernel map + empty user space.
int proc_clone_pagetable(pagetable_t **out_root) {
    pagetable_t *root = vm_create_root();
    if (!root) return -1;
    vm_clone_kernel_mappings(root);
    *out_root = root;
    return 0;
}

// Create a process with private code+stack (same VA, different PA).
// Seeds PCB with pc/usp/pagetable; seeding the kernel trap frame (ksp)
// is left to your existing proc/ctx code.
int proc_create(struct pcb *p) {
    // 1) Per-process root page table with kernel mappings
    if (proc_clone_pagetable(&p->pagetable) < 0) {
        printf("proc_create: no pagetable\n");
        return -1;
    }

    // 2) Allocate physical frames for user code and stack
    int f_code  = frame_alloc();
    int f_stack = frame_alloc();
    if (f_code < 0 || f_stack < 0) {
        printf("proc_create: no frames\n");
        if (f_code  >= 0) frame_release(f_code);
        if (f_stack >= 0) frame_release(f_stack);
        return -1;
    }
    uint32_t pa_code  = (uint32_t)(uintptr_t)FRAME(void, f_code);
    uint32_t pa_stack = (uint32_t)(uintptr_t)FRAME(void, f_stack);

    // 3) Map user code (RXU) at USER_BASE and 1 (or more) stack pages (RWU) below USER_TOP
    if (vm_map(p->pagetable, USER_BASE, pa_code,  PTE_R | PTE_X | PTE_U) < 0) return -1;
    if (vm_map(p->pagetable, USER_TOP - PAGE_SIZE, pa_stack, PTE_R | PTE_W | PTE_U) < 0) return -1;

    // 4) Copy the embedded user binary into the code page
    uint32_t u_size = (uint32_t)(_binary_user_bin_end - _binary_user_bin_start);
    if (u_size > PAGE_SIZE) {
        printf("user.bin too big: %u bytes\n", u_size);
        return -1;
    }
    uint8_t *dst = (uint8_t *)(uintptr_t)pa_code;  // copy to PA; mapping is identity in M-mode
    for (uint32_t i = 0; i < u_size; i++) dst[i] = _binary_user_bin_start[i];

    // 5) Initialize PCB’s user entry and user stack
    p->pc  = USER_BASE;
    p->usp = USER_TOP;

    // NOTE: If you “start” a process by context-switching to a
    // prebuilt trap frame (recommended), also seed that frame here:
    //    proc_seed_frame(p, p->pc, p->usp);

    return 0;
}
