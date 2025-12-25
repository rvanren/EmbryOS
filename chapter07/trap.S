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
    .align 2
_trap_handler:
    addi    sp, sp, -TRAP_FRAME_SIZE    # reserve trap frame

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
    li      t0, 0       // user sp
    REG_S   t0, 35*WORD_SIZE(sp)

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
    REG_L t6,  29*WORD_SIZE(sp)

    addi  sp, sp, TRAP_FRAME_SIZE   # remove trap frame
    sret

# void trap_wfi(block): wait for interrupt.
#       If block, wait until an interrupt is pending.  Then briefly enable
#       interrupts to invoke _trap_handler above.
    .globl trap_wfi
    .type  trap_wfi, @function
trap_wfi:
    la      t0, _trap_handler
    csrw    stvec, t0
    beqz    a0, 1f
    wfi                          # wait until interrupt is pending
1:  csrr    t0, sstatus          # read sstatus
    ori     t0, t0, (1 << 1)     # set SIE bit (bit 1)
    csrw    sstatus, t0          # write it back
    csrr    t0, sstatus          # read again
    li      t1, ~(1 << 1)        # mask with SIE cleared
    and     t0, t0, t1
    csrw    sstatus, t0
    ret
