# ctx_user(old_sp_ptr, new_sp, entry)
# Switch to a new stack and start running entry() in user mode.

    .section .text
    .globl ctx_user
ctx_user:
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
