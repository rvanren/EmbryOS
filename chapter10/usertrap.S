#if __riscv_xlen == 64
    #define REG_S sd
    #define REG_L ld
    #define WORD_SIZE 8
#else
    #define REG_S sw
    #define REG_L lw
    #define WORD_SIZE 4
#endif

#include "platform.h"

    .equ TRAP_REGS, 36
    .equ TRAP_FRAME_SIZE, TRAP_REGS * WORD_SIZE

    .section .text
    .align 2
_user_trap_handler:
    #
    # On entry from user mode:
    #   sp        = user stack pointer
    #   sscratch  = pcb + PAGE_SIZE - TRAP_FRAME_SIZE

    # --- Switch to kernel stack ---
    csrrw   sp, sscratch, sp        # sp = kernel_sp, sscratch = user_sp
    # --- Save user registers (including user tp) ---
    REG_S ra,   0*WORD_SIZE(sp)
    REG_S gp,   1*WORD_SIZE(sp)
    REG_S tp,   2*WORD_SIZE(sp)
    REG_S t0,   3*WORD_SIZE(sp)
    REG_S t1,   4*WORD_SIZE(sp)
    REG_S t2,   5*WORD_SIZE(sp)
    REG_S s0,   6*WORD_SIZE(sp)
    REG_S s1,   7*WORD_SIZE(sp)
    REG_S a0,   8*WORD_SIZE(sp)
    REG_S a1,   9*WORD_SIZE(sp)
    REG_S a2,  10*WORD_SIZE(sp)
    REG_S a3,  11*WORD_SIZE(sp)
    REG_S a4,  12*WORD_SIZE(sp)
    REG_S a5,  13*WORD_SIZE(sp)
    REG_S a6,  14*WORD_SIZE(sp)
    REG_S a7,  15*WORD_SIZE(sp)
    REG_S s2,  16*WORD_SIZE(sp)
    REG_S s3,  17*WORD_SIZE(sp)
    REG_S s4,  18*WORD_SIZE(sp)
    REG_S s5,  19*WORD_SIZE(sp)
    REG_S s6,  20*WORD_SIZE(sp)
    REG_S s7,  21*WORD_SIZE(sp)
    REG_S s8,  22*WORD_SIZE(sp)
    REG_S s9,  23*WORD_SIZE(sp)
    REG_S s10, 24*WORD_SIZE(sp)
    REG_S s11, 25*WORD_SIZE(sp)
    REG_S t3,  26*WORD_SIZE(sp)
    REG_S t4,  27*WORD_SIZE(sp)
    REG_S t5,  28*WORD_SIZE(sp)
    REG_S t6,  29*WORD_SIZE(sp)

    # --- Save CSRs ---
    csrr    t0, sepc
    REG_S   t0, 30*WORD_SIZE(sp)
    csrr    t0, sstatus
    REG_S   t0, 31*WORD_SIZE(sp)
    csrr    t0, scause
    REG_S   t0, 32*WORD_SIZE(sp)
    csrr    t0, stval
    REG_S   t0, 33*WORD_SIZE(sp)
    csrr    t0, stvec
    REG_S   t0, 34*WORD_SIZE(sp)

    # --- Save user_sp (currently in sscratch) ---
    csrr    t0, sscratch
    REG_S   t0, 35*WORD_SIZE(sp)

    # --- Recompute tp ---
    li      t0, PAGE_SIZE - TRAP_FRAME_SIZE
    sub     tp, sp, t0                # tp = pcb pointer

    # --- Call C handler ---
    mv      a0, sp
    call    software_trap_handler

    # --- Restore CSRs (except scause and stval) ---
    REG_L   t0, 30*WORD_SIZE(sp)
    csrw    sepc, t0
    REG_L   t0, 31*WORD_SIZE(sp)
    csrw    sstatus, t0
    REG_L   t0, 34*WORD_SIZE(sp)
    csrw    stvec, t0
    # No need to restore scause and stval

    # --- Restore user registers ---
    REG_L ra,   0*WORD_SIZE(sp)
    REG_L gp,   1*WORD_SIZE(sp)
    REG_L tp,   2*WORD_SIZE(sp)
    REG_L t0,   3*WORD_SIZE(sp)
    REG_L t1,   4*WORD_SIZE(sp)
    REG_L t2,   5*WORD_SIZE(sp)
    REG_L s0,   6*WORD_SIZE(sp)
    REG_L s1,   7*WORD_SIZE(sp)
    REG_L a0,   8*WORD_SIZE(sp)
    REG_L a1,   9*WORD_SIZE(sp)
    REG_L a2,  10*WORD_SIZE(sp)
    REG_L a3,  11*WORD_SIZE(sp)
    REG_L a4,  12*WORD_SIZE(sp)
    REG_L a5,  13*WORD_SIZE(sp)
    REG_L a6,  14*WORD_SIZE(sp)
    REG_L a7,  15*WORD_SIZE(sp)
    REG_L s2,  16*WORD_SIZE(sp)
    REG_L s3,  17*WORD_SIZE(sp)
    REG_L s4,  18*WORD_SIZE(sp)
    REG_L s5,  19*WORD_SIZE(sp)
    REG_L s6,  20*WORD_SIZE(sp)
    REG_L s7,  21*WORD_SIZE(sp)
    REG_L s8,  22*WORD_SIZE(sp)
    REG_L s9,  23*WORD_SIZE(sp)
    REG_L s10, 24*WORD_SIZE(sp)
    REG_L s11, 25*WORD_SIZE(sp)
    REG_L t3,  26*WORD_SIZE(sp)
    REG_L t4,  27*WORD_SIZE(sp)
    REG_L t5,  28*WORD_SIZE(sp)
    REG_L t6,  29*WORD_SIZE(sp)     # restore t6
    csrw sscratch, sp               # sscratch = kernel sp
    REG_L sp,  35*WORD_SIZE(sp)     # restore user stack pointer
    sret

    .globl enter_user
    .type  enter_user, @function

# void enter_user(uintptr_t entry, uintptr_t user_sp, uintptr_t ksp)
# a0=entry, a1=user_sp, a2=ksp
enter_user:
    # use arguments now
    csrw  sepc, a0        # user entry PC
    mv   sp, a1           # user sp
    csrw sscratch, a2     # kernel stack pointer for future traps

    # set trap vector to _user_trap_handler
    la   t0, _user_trap_handler
    csrw stvec, t0 

    # switch privilege to U-mode in sstatus register
    csrr  t0, sstatus
    li    t1, (1 << 8)    # SSTATUS_SPP bit
    not   t2, t1
    and   t0, t0, t2      # clear SPP -> next = U
    csrw  sstatus, t0

    # clear all registers except a0, a1, and sp
    li gp, 0
    li ra, 0
    li tp, 0
    li t0, 0
    li t1, 0
    li t2, 0
    li t3, 0
    li t4, 0
    li t5, 0
    li t6, 0
    li s0, 0
    li s1, 0
    li s2, 0
    li s3, 0
    li s4, 0
    li s5, 0
    li s6, 0
    li s7, 0
    li s8, 0
    li s9, 0
    li s10, 0
    li s11, 0
    li a2, 0
    li a3, 0
    li a4, 0
    li a5, 0
    li a6, 0
    li a7, 0

    # enter user mode
    sret
