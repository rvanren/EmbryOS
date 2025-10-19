#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>

#define TRAP_FRAME_SIZE 128                 // 32 4-byte registers

struct trap_frame {
    uint32_t ra, sp, gp, tp;                // special purpose registers
    uint32_t t0, t1, t2;                    // caller-saved temps
    uint32_t s0, s1;                        // callee-saved
    uint32_t a0, a1, a2, a3, a4, a5, a6, a7;    // syscall arguments & results
    uint32_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;  // callee-saved
    uint32_t t3, t4, t5, t6;                // caller-saved
    uint32_t mepc;                          // saved program counter
};

#endif
