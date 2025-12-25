#pragma once

#include "types.h"

#define TRAP_REGS       36
#define TRAP_FRAME_SIZE (TRAP_REGS * sizeof(uword_t))

// These are all the registers saved when an interrupt occurs
struct trap_frame {
    uword_t ra, gp, tp;
    uword_t t0, t1, t2;
    uword_t s0, s1;
    uword_t a0, a1, a2, a3, a4, a5, a6, a7;
    uword_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    uword_t t3, t4, t5, t6;
    uword_t sepc, sstatus, scause, stval, stvec;
    uword_t usp;
};

extern void trap_wfi(uword_t block);     // defined in assembly
