# ================================================================
# S-mode Trap Handler for EmbryOS (RV32 / RV64, under OpenSBI)
# ================================================================
#   - Saves all integer registers (x1–x31)
#   - Saves S-mode CSRs (sepc, sstatus, scause, stval)
#   - Calls C: software_trap_handler(struct trap_frame *tf)
#   - Restores and sret's
# ================================================================

    .section .text
    .globl _trap_handler

# ----------------------------------------------------------------
# Define width-dependent macros
# ----------------------------------------------------------------
#if __riscv_xlen == 64
    # 8-byte words
    # define store/load instructions
    .set REG_S, sd
    .set REG_L, ld
    .set WORD_SIZE, 8
#else
    # 4-byte words
    .set REG_S, sw
    .set REG_L, lw
    .set WORD_SIZE, 4
#endif

    .equ TRAP_REGS, 36
    .equ TRAP_FRAME_SIZE, TRAP_REGS * WORD_SIZE

_trap_handler:
    # ------------------------------------------------------------
    # Allocate trap frame (keep SP 16-byte aligned)
    # ------------------------------------------------------------
    addi sp, sp, -TRAP_FRAME_SIZE
    andi sp, sp, -16

    # --- save all general registers ---
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

    # usp (user stack pointer) placeholder
    REG_S zero, 35*WORD_SIZE(sp)

    # ------------------------------------------------------------
    # Call C-level trap handler
    # ------------------------------------------------------------
    mv a0, sp                   # a0 = &trap_frame
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
