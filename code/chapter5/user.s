# ctx_user(old_sp_ptr, new_sp, entry)
# Switch to a new stack and start running entry() in user mode.

    .section .text
    .globl ctx_user
ctx_user:
    addi sp, sp, -64
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)
    sw s11, 48(sp)
    sw ra, 52(sp)

    # a0 = &old_sp, a1 = new_sp, a2 = entry
    sw  sp, 0(a0)           # save current sp
    mv  sp, a1              # switch to new stack

    csrw mepc, a2           # set return address (entry point)

    csrr t0, mstatus
    li   t1, ~(3 << 11)     # clear MPP bits
    and  t0, t0, t1
    or   t0, t0, (0 << 11)  # set MPP = 00 (U-mode)
    csrw mstatus, t0

    mret                    # jump to user entry, now in U-mode
