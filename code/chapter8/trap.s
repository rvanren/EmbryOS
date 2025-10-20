    .equ TRAP_FRAME_SIZE, 144

    .section .text
    .globl _trap_handler                                                            .align 2
_trap_handler:
    /* If trap came from user mode (MPP==0), swap sp with mscratch. */
    csrr   t0, mstatus
    li     t1, (3 << 11)       /* mask for MPP bits */
    and    t0, t0, t1
    beqz   t0, 1f              /* if MPP==0 -> from user */
    j      2f                  /* else from kernel, skip swap */

1:  csrrw  sp, mscratch, sp    /* swap user<->kernel stacks */

2:  addi   sp, sp, -TRAP_FRAME_SIZE

    /* save general registers */
    sw     ra,   0(sp)
    sw     sp,   4(sp)
    sw     gp,   8(sp)
    sw     tp,   12(sp)
    sw     t0,   16(sp)
    sw     t1,   20(sp)
    sw     t2,   24(sp)
    sw     s0,   28(sp)
    sw     s1,   32(sp)
    sw     a0,   36(sp)
    sw     a1,   40(sp)
    sw     a2,   44(sp)
    sw     a3,   48(sp)
    sw     a4,   52(sp)
    sw     a5,   56(sp)
    sw     a6,   60(sp)
    sw     a7,   64(sp)
    sw     s2,   68(sp)
    sw     s3,   72(sp)
    sw     s4,   76(sp)
    sw     s5,   80(sp)
    sw     s6,   84(sp)
    sw     s7,   88(sp)
    sw     s8,   92(sp)
    sw     s9,   96(sp)
    sw     s10,  100(sp)
    sw     s11,  104(sp)
    sw     t3,   108(sp)
    sw     t4,   112(sp)
    sw     t5,   116(sp)
    sw     t6,   120(sp)

    /* save CSRs */
    csrr   t0, mepc
    sw     t0, 124(sp)
    csrr   t0, mstatus
    sw     t0, 128(sp)
    csrr   t0, mcause
    sw     t0, 132(sp)
    csrr   t0, mtval
    sw     t0, 136(sp)

    /* If we trapped from user mode, mscratch currently holds the user sp. Save it. */
    csrr   t0, mstatus
    li     t1, (3 << 11)
    and    t0, t0, t1
    beqz   t0, 3f
    j      4f
3:  csrr   t2, mscratch
    sw     t2, 140(sp)         /* tf->usp */
4:

    /* call C handler: a0 = &trap_frame */
    mv     a0, sp
    call   software_trap_handler

    /* --- restore registers --- */
    lw     ra,   0(sp)
    lw     sp,   4(sp)
    lw     gp,   8(sp)
    lw     tp,   12(sp)
    lw     t0,   16(sp)
    lw     t1,   20(sp)
    lw     t2,   24(sp)
    lw     s0,   28(sp)
    lw     s1,   32(sp)
    lw     a0,   36(sp)
    lw     a1,   40(sp)
    lw     a2,   44(sp)
    lw     a3,   48(sp)
    lw     a4,   52(sp)
    lw     a5,   56(sp)
    lw     a6,   60(sp)
    lw     a7,   64(sp)
    lw     s2,   68(sp)
    lw     s3,   72(sp)
    lw     s4,   76(sp)
    lw     s5,   80(sp)
    lw     s6,   84(sp)
    lw     s7,   88(sp)
    lw     s8,   92(sp)
    lw     s9,   96(sp)
    lw     s10,  100(sp)
    lw     s11,  104(sp)
    lw     t3,   108(sp)
    lw     t4,   112(sp)
    lw     t5,   116(sp)
    lw     t6,   120(sp)

    /* restore mepc, mstatus */
    lw     t0, 124(sp)
    csrw   mepc, t0
    lw     t0, 128(sp)
    csrw   mstatus, t0

    /* returning to user? (MPP==0) */
    li     t1, (3 << 11)
    and    t0, t0, t1
    beqz   t0, 5f
    j      6f

5:  /* put user sp back into mscratch for next trap */
    lw     t2, 140(sp)
    csrw   mscratch, t2
    addi   sp, sp, TRAP_FRAME_SIZE
    csrrw  sp, mscratch, sp     /* swap back: sp=user, mscratch=kernel */
    mret

6:  /* trap from kernel; just pop frame and return */
    addi   sp, sp, TRAP_FRAME_SIZE
    mret
