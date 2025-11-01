# trap.S -- portable trap handler for RV32 / RV64
# Swap stacks if trap from user, save full trap frame, call C handler.
# Must be saved as trap.S (capital S) so preprocessor runs.

#if __riscv_xlen == 64
    #define REG_S sd
    #define REG_L ld
    #define WORD_SIZE 8
#else
    #define REG_S sw
    #define REG_L lw
    #define WORD_SIZE 4
#endif

    .equ TRAP_REGS, 36
    .equ TRAP_FRAME_SIZE, TRAP_REGS * WORD_SIZE

    .section .text
    .globl _trap_handler
    .align 2
_trap_handler:
    # If trap came from user mode (SPP==0), swap sp with sscratch
    csrr   t0, sstatus
    li     t1, (1 << 8)          # SPP mask
    and    t0, t1, t0
    bnez   t0, 1f                # if SPP!=0 -> kernel trap (skip swap)
    # swap: old sscratch -> sp, old sp -> sscratch
    csrrw  sp, sscratch, sp

1:  # allocate trap frame (on current stack: kernel stack if we swapped)
    addi   sp, sp, -TRAP_FRAME_SIZE

    # --- save general registers ---
    REG_S ra,   0*WORD_SIZE(sp)
    REG_S sp,   1*WORD_SIZE(sp)
    REG_S gp,   2*WORD_SIZE(sp)
    REG_S tp,   3*WORD_SIZE(sp)
    REG_S t0,   4*WORD_SIZE(sp)
    REG_S t1,   5*WORD_SIZE(sp)
    REG_S t2,   6*WORD_SIZE(sp)
    REG_S s0,   7*WORD_SIZE(sp)
    REG_S s1,   8*WORD_SIZE(sp)
    REG_S a0,   9*WORD_SIZE(sp)
    REG_S a1,  10*WORD_SIZE(sp)
    REG_S a2,  11*WORD_SIZE(sp)
    REG_S a3,  12*WORD_SIZE(sp)
    REG_S a4,  13*WORD_SIZE(sp)
    REG_S a5,  14*WORD_SIZE(sp)
    REG_S a6,  15*WORD_SIZE(sp)
    REG_S a7,  16*WORD_SIZE(sp)
    REG_S s2,  17*WORD_SIZE(sp)
    REG_S s3,  18*WORD_SIZE(sp)
    REG_S s4,  19*WORD_SIZE(sp)
    REG_S s5,  20*WORD_SIZE(sp)
    REG_S s6,  21*WORD_SIZE(sp)
    REG_S s7,  22*WORD_SIZE(sp)
    REG_S s8,  23*WORD_SIZE(sp)
    REG_S s9,  24*WORD_SIZE(sp)
    REG_S s10, 25*WORD_SIZE(sp)
    REG_S s11, 26*WORD_SIZE(sp)
    REG_S t3,  27*WORD_SIZE(sp)
    REG_S t4,  28*WORD_SIZE(sp)
    REG_S t5,  29*WORD_SIZE(sp)
    REG_S t6,  30*WORD_SIZE(sp)

    # --- save CSRs into frame ---
    csrr   t0, sepc
    REG_S t0, 31*WORD_SIZE(sp)
    csrr   t0, sstatus
    REG_S t0, 32*WORD_SIZE(sp)
    csrr   t0, scause
    REG_S t0, 33*WORD_SIZE(sp)
    csrr   t0, stval
    REG_S t0, 34*WORD_SIZE(sp)

    # If trap from user, sscratch now holds user_sp (because we swapped earlier).
    # Save usp (user sp) in frame; otherwise store zero.
    csrr   t0, sstatus
    li     t1, (1 << 8)
    and    t0, t1, t0
    bnez   t0, 2f
    csrr   t2, sscratch
    REG_S t2, 35*WORD_SIZE(sp)

2:  # Call C-level handler: software_trap_handler(struct trap_frame *tf)
    mv     a0, sp
    call   software_trap_handler

    # --- restore CSRs ---
    REG_L t0, 31*WORD_SIZE(sp)
    csrw   sepc, t0
    REG_L t0, 32*WORD_SIZE(sp)
    csrw   sstatus, t0

    # --- restore general registers ---
    REG_L ra,   0*WORD_SIZE(sp)
    REG_L sp,   1*WORD_SIZE(sp)
    REG_L gp,   2*WORD_SIZE(sp)
    REG_L tp,   3*WORD_SIZE(sp)
    REG_L t0,   4*WORD_SIZE(sp)
    REG_L t1,   5*WORD_SIZE(sp)
    REG_L t2,   6*WORD_SIZE(sp)
    REG_L s0,   7*WORD_SIZE(sp)
    REG_L s1,   8*WORD_SIZE(sp)
    REG_L a0,   9*WORD_SIZE(sp)
    REG_L a1,  10*WORD_SIZE(sp)
    REG_L a2,  11*WORD_SIZE(sp)
    REG_L a3,  12*WORD_SIZE(sp)
    REG_L a4,  13*WORD_SIZE(sp)
    REG_L a5,  14*WORD_SIZE(sp)
    REG_L a6,  15*WORD_SIZE(sp)
    REG_L a7,  16*WORD_SIZE(sp)
    REG_L s2,  17*WORD_SIZE(sp)
    REG_L s3,  18*WORD_SIZE(sp)
    REG_L s4,  19*WORD_SIZE(sp)
    REG_L s5,  20*WORD_SIZE(sp)
    REG_L s6,  21*WORD_SIZE(sp)
    REG_L s7,  22*WORD_SIZE(sp)
    REG_L s8,  23*WORD_SIZE(sp)
    REG_L s9,  24*WORD_SIZE(sp)
    REG_L s10, 25*WORD_SIZE(sp)
    REG_L s11, 26*WORD_SIZE(sp)
    REG_L t3,  27*WORD_SIZE(sp)
    REG_L t4,  28*WORD_SIZE(sp)
    REG_L t5,  29*WORD_SIZE(sp)
    REG_L t6,  30*WORD_SIZE(sp)

    # --- decide where to return ---
    REG_L t0, 32*WORD_SIZE(sp)        # saved sstatus
    li     t1, (1 << 8)               # SPP bit mask
    and    t0, t0, t1
    bnez   t0, 3f                     # if SPP!=0 -> kernel trap

    # returning to user
    REG_L t2, 35*WORD_SIZE(sp)        # tf->usp (user sp)
    csrw   sscratch, t2               # restore sscratch to hold user_sp
    addi   sp, sp, TRAP_FRAME_SIZE
    csrrw  sp, sscratch, sp           # swap back: sp=old sscratch (user sp), sscratch=old sp
    sret

3:  addi   sp, sp, TRAP_FRAME_SIZE
    sret
