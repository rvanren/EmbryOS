// chapter4/timer.c
#include <stdint.h>
#include "process.h"

// QEMU virt CLINT (machine timer)
// mtime @ 0x0200_BFF8 (64-bit counter)
// mtimecmp[h] base @ 0x0200_4000, stride 8 bytes per hart
#define CLINT_BASE      0x02000000UL
#define MTIMECMP_BASE   (CLINT_BASE + 0x4000)
#define MTIME_ADDR      (CLINT_BASE + 0xBFF8)

#define MTIE_MASK       (1u << 7)   // mie.MTIE
#define MIE_MASK        (1u << 3)   // mstatus.MIE

// Pick a tick interval that's visible but not too slow.
// Tweak to taste; QEMU's virtual rate is implementation-dependent.
#define TICK_INTERVAL   (100000u)

static inline uint32_t read_csr_mhartid(void) {
    uint32_t x;
    __asm__ volatile ("csrr %0, mhartid" : "=r"(x));
    return x;
}

static inline void write_csr_mtvec(void *handler) {
    uintptr_t addr = (uintptr_t)handler;      // direct mode (MODE=0)
    __asm__ volatile ("csrw mtvec, %0" :: "r"(addr));
}

static inline void set_csr(uint32_t csr, uint32_t mask) {
    switch (csr) {
      case 0x304: __asm__ volatile ("csrs mie, %0"     :: "r"(mask)); break;
      case 0x300: __asm__ volatile ("csrs mstatus, %0" :: "r"(mask)); break;
    }
}

static inline uint64_t mtime_read(void) {
    volatile uint64_t *mtime = (volatile uint64_t *)(MTIME_ADDR);
    return *mtime;  // RV32 toolchain emits two 32b ops
}

static inline void mtimecmp_write(uint32_t hart, uint64_t val) {
    volatile uint64_t *mtimecmp = (volatile uint64_t *)(MTIMECMP_BASE + 8u * hart);
    *mtimecmp = val;  // RV32: compiled as two 32b stores (hi/lo) with correct order
}

// Called from trap.s once at boot to install trap handler and start the timer.
extern void trap_entry(void);

void timer_init(void) {
    uint32_t hart = read_csr_mhartid();

    // Route all traps/interrupts to our assembly entry.
    write_csr_mtvec((void*)trap_entry);

    // Program first tick.
    uint64_t now = mtime_read();
    mtimecmp_write(hart, now + TICK_INTERVAL);

    // Enable machine-timer interrupt and global MIE.
    set_csr(0x304, MTIE_MASK);  // mie.MTIE = 1
    set_csr(0x300, MIE_MASK);   // mstatus.MIE = 1
}

// C-side timer ISR body, called by trap handler when mcause == machine-timer.
void timer_interrupt(void) {
    uint32_t hart = read_csr_mhartid();

    // Re-arm next tick *before* switching away (avoid retrigger loops).
    uint64_t now = mtime_read();
    mtimecmp_write(hart, now + TICK_INTERVAL);

    // Preempt: hand CPU to the next process.
    proc_yield();
}
