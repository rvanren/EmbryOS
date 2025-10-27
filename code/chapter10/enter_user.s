    .section .text
    .globl enter_user
    .type  enter_user,@function
# void enter_user(void *entry, uintptr_t gp_val,
#                 uintptr_t user_sp, size_t arg_size, uintptr_t ksp)
# a0=entry, a1=gp_val, a2=user_sp, a3=arg_size, a4=ksp
enter_user:
    # Save incoming args into safe temps
    mv   s2, a0           # entry
    mv   s3, a1           # gp_val
    mv   s4, a2           # user_sp (points to args)
    mv   s5, a3           # arg_size
    mv   s6, a4           # ksp

    # Program per-process kernel stack for future traps
    csrw mscratch, s6

    # Disable M-mode interrupts during handoff
    csrrc t0, mstatus, (1 << 3)

    # Zero all integer regs except gp/sp and the temps we still need
    li   t0, 0
    mv   ra, t0
    mv   tp, t0
    mv   t1, t0
    mv   t2, t0
    mv   s0, t0
    mv   s1, t0
    mv   a0, t0
    mv   a1, t0
    mv   a2, t0
    mv   a3, t0
    mv   a4, t0
    mv   a5, t0
    mv   a6, t0
    mv   a7, t0
    # leave s2–s6 intact

    # Load user gp/sp
    mv   gp, s3
    mv   sp, s4

    # Initialize user arguments
    mv   a0, s4           # arg_buf pointer
    mv   a1, s5           # arg_buf size

    # Switch privilege: MPP=U, mepc=entry
    csrr t0, mstatus
    li   t1, 0x1800                 # MSTATUS_MPP_MASK (bits 12..11)
    not  t2, t1
    and  t0, t0, t2                 # clear MPP -> U
    csrw mstatus, t0
    csrw mepc, s2

    mret
