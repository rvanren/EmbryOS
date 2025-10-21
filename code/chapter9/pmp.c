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

static inline uintptr_t pmp_napot_addr(uintptr_t base, size_t size) {
  return (base >> 2) | ((size >> 1) - 1);
}

void pmp_config(struct pcb *pcb) {
    pcb->pmp.addr[0] = pmp_napot_addr((uintptr_t) pcb->base, PAGE_SIZE);
    pcb->pmp.addr[1] = pmp_napot_addr((uintptr_t) pcb->stack, PAGE_SIZE);
    pcb->pmp.cfg[0]  = PMP_A_NAPOT | PMP_R | PMP_W | PMP_X;
    pcb->pmp.cfg[1]  = PMP_A_NAPOT | PMP_R | PMP_W;
}

// Set PMP registers (before each mret)
void pmp_load(struct pcb *pcb) {
    write_csr(pmpaddr0, pcb->pmp.addr[0]);
    write_csr(pmpaddr1, pcb->pmp.addr[1]);
    write_csr(pmpcfg0, cfg = ((uintptr_t) pcb->pmp.cfg[0]) |
                        ((uintptr_t) pcb->pmp.cfg[1] << 8));
}
