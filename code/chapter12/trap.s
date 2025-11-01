    .equ TRAP_FRAME_SIZE, 144   # 36 regs × 4 bytes

    .section .text
    .globl _trap_handler
    .align 2
_trap_handler:
    # If trap came from user mode (SPP==0), swap sp with sscratch
    csrr   t0, sstatus
    li     t1, (1 << 8)          # mask for SPP bit
    and    t0, t1, t0
    bnez   t0, 1f                # if SPP!=0 → kernel trap
    csrrw  sp, sscratch, sp      # swap stacks (user↔kernel)

1:  addi   sp, sp, -TRAP_FRAME_SIZE

    # --- save general registers ---
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

    # --- save CSRs ---
    csrr   t0, sepc
    sw     t0, 124(sp)
    csrr   t0, sstatus
    sw     t0, 128(sp)
    csrr   t0, scause
    sw     t0, 132(sp)
    csrr   t0, stval
    sw     t0, 136(sp)

    # If trap from user (SPP==0), sscratch currently holds user sp; save it.
    csrr   t0, sstatus
    li     t1, (1 << 8)
    and    t0, t1, t0
    bnez   t0, 2f
    csrr   t2, sscratch
    sw     t2, 140(sp)           # tf->usp

2:  # Call C-level handler
    mv     a0, sp
    call   software_trap_handler

    # --- restore CSRs ---
    lw     t0, 124(sp)
    csrw   sepc, t0
    lw     t0, 128(sp)
    csrw   sstatus, t0

    # --- restore registers ---
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

    # --- decide where to return ---
    lw     t0, 128(sp)           # sstatus
    li     t1, (1 << 8)          # SPP bit
    and    t0, t0, t1
    bnez   t0, 3f                # if SPP!=0 -> kernel trap

    # returning to user
    lw     t2, 140(sp)
    csrw   sscratch, t2
    addi   sp, sp, TRAP_FRAME_SIZE
    csrrw  sp, sscratch, sp
    sret

3:  addi   sp, sp, TRAP_FRAME_SIZE
    sret
