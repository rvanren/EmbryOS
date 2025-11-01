# ================================================================
#  EmbryOS Trap Handler (RV32 / RV64 under OpenSBI)
# ================================================================
#  Saves all general-purpose registers and supervisor CSRs in a
#  struct trap_frame, calls software_trap_handler(tf), restores,
#  and returns with sret.
# ================================================================

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
_trap_handler:
    # ------------------------------------------------------------
    # Allocate trap frame and align stack
    # ------------------------------------------------------------
    addi sp, sp, -TRAP_FRAME_SIZE
    andi sp, sp, -16

    # --- Save general registers (x1–x31) ---
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

    # ------------------------------------------------------------
    # Save supervisor CSRs
    # ------------------------------------------------------------
    csrr t0, sepc
    REG_S t0, 31*WORD_SIZE(sp)
    csrr t0, sstatus
    REG_S t0, 32*WORD_SIZE(sp)
    csrr t0, scause
    REG_S t0, 33*WORD_SIZE(sp)
    csrr t0, stval
    REG_S t0, 34*WORD_SIZE(sp)

    # usp (user stack pointer, unused yet)
    REG_S zero, 35*WORD_SIZE(sp)

    # ------------------------------------------------------------
    # Call C trap handler: software_trap_handler(struct trap_frame *tf)
    # ------------------------------------------------------------
    mv a0, sp
    call software_trap_handler

    # ------------------------------------------------------------
    # Restore CSRs
    # ------------------------------------------------------------
    REG_L t0, 31*WORD_SIZE(sp)
    csrw sepc, t0
    REG_L t0, 32*WORD_SIZE(sp)
    csrw sstatus, t0

    # ------------------------------------------------------------
    # Restore general registers
    # ------------------------------------------------------------
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

    # ------------------------------------------------------------
    # Free trap frame and return
    # ------------------------------------------------------------
    addi sp, sp, TRAP_FRAME_SIZE
    sret
