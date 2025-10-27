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

static inline uintptr_t pmp_napot_addr(uintptr_t base, uintptr_t size) {
  return (base >> 2) | ((size - 1) >> 3);
}

void pmp_init(void) {
    write_csr(pmpaddr0, pmp_napot_addr(0x80000000, 0x400000)); 
    write_csr(pmpcfg0, (uintptr_t) (PMP_A_NAPOT | PMP_R | PMP_W | PMP_X));
}
