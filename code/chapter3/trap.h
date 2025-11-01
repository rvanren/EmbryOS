#pragma once

#include "platform.h"

// ----------------------------------------------------------------------
// Trap frame: all general-purpose registers saved on trap entry.
// ----------------------------------------------------------------------

#define TRAP_REGS       36
#define TRAP_FRAME_SIZE (TRAP_REGS * sizeof(uword_t))

struct trap_frame {
    uword_t ra, sp, gp, tp;                          //  0–3
    uword_t t0, t1, t2;                              //  4–6
    uword_t s0, s1;                                  //  7–8
    uword_t a0, a1, a2, a3, a4, a5, a6, a7;          //  9–16
    uword_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;// 17–26
    uword_t t3, t4, t5, t6;                          // 27–30
    uword_t sepc;                                    // 31
    uword_t sstatus;                                 // 32
    uword_t scause;                                  // 33
    uword_t stval;                                   // 34
    uword_t usp;                                     // 35 (user sp)
};
