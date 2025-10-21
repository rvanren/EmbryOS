// CSR helpers (if you don’t already have them)
#define read_csr(reg) ({ unsigned long __tmp; \
  __asm__ volatile ("csrr %0, " #reg : "=r"(__tmp)); __tmp; })
#define write_csr(reg, val) ({ \
  __asm__ volatile ("csrw " #reg ", %0" :: "rK"(val)); })

// PMP config bits
enum {
  PMP_R = 1 << 0,
  PMP_W = 1 << 1,
  PMP_X = 1 << 2,
  PMP_A_NAPOT = 0b11 << 3,
  PMP_L = 1 << 7,
};

// Encode NAPOT pmpaddr from base/size (size power-of-two >= 8)
static inline unsigned long pmp_napot_addr(uintptr_t base, size_t size) {
  return (base >> 2) | ((size >> 1) - 1);
}

#ifdef notdef
// Set cfg byte i in pmpcfg0 (entries 0..3)
static inline void pmpcfg0_set_byte(int i, unsigned char cfg) {
  unsigned long c = read_csr(pmpcfg0);
  unsigned shift = (i & 3) * 8;
  c &= ~(0xFFUL << shift);
  c |= ((unsigned long)cfg) << shift;
  write_csr(pmpcfg0, c);
}
#endif

// Compute the PMP configuration for a process
void pmp_config(struct pcb *p, void *base, void *stack) {
    p->pmp.addr[0] = pmp_napot_addr((uintptr_t) base, PAGE_SIZE);
    p->pmp.addr[1] = pmp_napot_addr((uintptr_t) stack, PAGE_SIZE);
    p->pmp.cfg[0]  = PMP_A_NAPOT | PMP_R | PMP_W | PMP_X;
    p->pmp.cfg[1]  = PMP_A_NAPOT | PMP_R | PMP_W;
}

// Set PMP registers (before each mret)
void pmp_load(struct pcb *p) {
    // write pmpaddr registers
    write_csr(pmpaddr0, p->pmp.addr[0]);
    write_csr(pmpaddr1, p->pmp.addr[1]);

    // write pmpcfg0 (assume only using entries 0–1)
    unsigned long cfg = ((unsigned long)p->pmp.cfg[0]) |
                        ((unsigned long)p->pmp.cfg[1] << 8);
    write_csr(pmpcfg0, cfg);
}
