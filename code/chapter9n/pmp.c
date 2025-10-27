#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "process.h"

#define read_csr(reg) ({ uintptr_t __tmp; \
  __asm__ volatile ("csrr %0, " #reg : "=r"(__tmp)); __tmp; })
#define write_csr(reg, val) ({ \
  __asm__ volatile ("csrw " #reg ", %0" :: "rK"(val)); })

enum {
  PMP_R = 1 << 0,
  PMP_W = 1 << 1,
  PMP_X = 1 << 2,
  PMP_A_NAPOT = 0b11 << 3,
  PMP_L = 1 << 7,
};

static inline uintptr_t pmp_napot_addr(uintptr_t base) {
  return (base >> 2) | ((PAGE_SIZE - 1) >> 3);
}

#ifdef CH10
// Set PMP registers (before each mret)
void pmp_load(struct pcb *pcb) {
#ifndef NO_PMP
    write_csr(pmpaddr0, pmp_napot_addr((uintptr_t) pcb->base)); 
    write_csr(pmpaddr1, pmp_napot_addr((uintptr_t) pcb->stack)); 
    write_csr(pmpcfg0, ((uintptr_t) (PMP_A_NAPOT | PMP_R | PMP_W | PMP_X)) |
                        ((uintptr_t) (PMP_A_NAPOT | PMP_R | PMP_W) << 8));
#endif
}
#endif

void pmp_init() {
    extern user_setup();
    user_setup();
}
