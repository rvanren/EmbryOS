#pragma once

#include <stdint.h>

// When an interrupt or exception occurs, all registers are saved on the stack
// in a "trap frame".  We define here what exactly is being saved and in what
// position on the stack.  For example, when a system call is performed, we
// need to be able to extract the arguments.  We also sometimes need to change
// registers, for example to return the value of a system call.

#define TRAP_FRAME_SIZE 144   // 36 words x 4 bytes

struct trap_frame {
    uint32_t ra, sp, gp, tp;                 //  0 - 3
    uint32_t t0, t1, t2;                     //  4 - 6
    uint32_t s0, s1;                         //  7 - 8
    uint32_t a0, a1, a2, a3, a4, a5, a6, a7; //  9 - 16
    uint32_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11; // 17 - 26
    uint32_t t3, t4, t5, t6;                 // 27 - 30
    uint32_t sepc;                           // 31
    uint32_t sstatus;                        // 32
    uint32_t scause;                         // 33
    uint32_t stval;                          // 34
    uint32_t usp;                            // 35
};
