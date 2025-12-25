#if __riscv_xlen == 64
    #define REG_S sd
    #define REG_L ld
    #define WORD_SIZE 8
#else
    #define REG_S sw
    #define REG_L lw
    #define WORD_SIZE 4
#endif

    .section .text
    .globl ctx_switch, ctx_start

# void ctx_switch(void **old_sp, void *new_sp)
#   Saves current context into *old_sp, switches to new_sp, restores.
ctx_switch:
    addi sp, sp, -(16*WORD_SIZE)

    REG_S s0,  1*WORD_SIZE(sp)
    REG_S s1,  2*WORD_SIZE(sp)
    REG_S s2,  3*WORD_SIZE(sp)
    REG_S s3,  4*WORD_SIZE(sp)
    REG_S s4,  5*WORD_SIZE(sp)
    REG_S s5,  6*WORD_SIZE(sp)
    REG_S s6,  7*WORD_SIZE(sp)
    REG_S s7,  8*WORD_SIZE(sp)
    REG_S s8,  9*WORD_SIZE(sp)
    REG_S s9, 10*WORD_SIZE(sp)
    REG_S s10,11*WORD_SIZE(sp)
    REG_S s11,12*WORD_SIZE(sp)
    REG_S ra,  0*WORD_SIZE(sp)

    REG_S sp, 0(a0)          # *old_sp = current sp
    mv sp, a1                # switch to new stack

    REG_L ra,  0*WORD_SIZE(sp)
    REG_L s0,  1*WORD_SIZE(sp)
    REG_L s1,  2*WORD_SIZE(sp)
    REG_L s2,  3*WORD_SIZE(sp)
    REG_L s3,  4*WORD_SIZE(sp)
    REG_L s4,  5*WORD_SIZE(sp)
    REG_L s5,  6*WORD_SIZE(sp)
    REG_L s6,  7*WORD_SIZE(sp)
    REG_L s7,  8*WORD_SIZE(sp)
    REG_L s8,  9*WORD_SIZE(sp)
    REG_L s9, 10*WORD_SIZE(sp)
    REG_L s10,11*WORD_SIZE(sp)
    REG_L s11,12*WORD_SIZE(sp)

    addi sp, sp, (16*WORD_SIZE)
    ret

# void ctx_start(void **save_sp, void *new_sp)
#   Saves current sp, switches to new stack, and calls exec_user().
ctx_start:
    addi sp, sp, -(16*WORD_SIZE)

    REG_S s0,  1*WORD_SIZE(sp)
    REG_S s1,  2*WORD_SIZE(sp)
    REG_S s2,  3*WORD_SIZE(sp)
    REG_S s3,  4*WORD_SIZE(sp)
    REG_S s4,  5*WORD_SIZE(sp)
    REG_S s5,  6*WORD_SIZE(sp)
    REG_S s6,  7*WORD_SIZE(sp)
    REG_S s7,  8*WORD_SIZE(sp)
    REG_S s8,  9*WORD_SIZE(sp)
    REG_S s9, 10*WORD_SIZE(sp)
    REG_S s10,11*WORD_SIZE(sp)
    REG_S s11,12*WORD_SIZE(sp)
    REG_S ra,  0*WORD_SIZE(sp)

    REG_S sp, 0(a0)          # *save_sp = current sp
    mv sp, a1                # switch to new stack
    la a0, exec_user
    jalr a0                  # call exec_user() (doesn't return)
