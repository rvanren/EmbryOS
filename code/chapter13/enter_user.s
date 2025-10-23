    .section .text
    .globl enter_user
    .type  enter_user,@function
# void enter_user(void *entry, uintptr_t gp_val,
#                 uintptr_t user_sp, size_t arg_size, uintptr_t ksp)
# a0=entry, a1=gp_val, a2=user_sp, a3=arg_size, a4=ksp
enter_user:
    # Save incoming args into temps BEFORE clobbering aX
    mv   t3, a0           # entry
    mv   t4, a1           # gp_val
    mv   t5, a2           # user_sp (points to args)
    mv   t7, a3           # arg_size
    mv   t6, a4           # ksp

    # Program per-process kernel stack for future traps
    csrw mscratch, t6

    # Disable M-mode interrupts during handoff
    csrrc t0, mstatus, (1 << 3)

    # Zero all integer regs except gp/sp and temps
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
    mv   s2, t0
    mv   s3, t0
    mv   s4, t0
    mv   s5, t0
    mv   s6, t0
    mv   s7, t0
    mv   s8, t0
    mv   s9, t0
    mv   s10, t0
    mv   s11, t0
    # DO NOT clobber t3..t7 (they hold entry,gp,sp,ksp,arg_size)

    # Load user gp/sp
    mv   gp, t4
    mv   sp, t5

    # Initialize user arguments
    mv   a0, t5           # arg_buf pointer
    mv   a1, t7           # arg_buf size

    # Switch privilege: MPP=U, mepc=entry
    csrr t0, mstatus
    li   t1, 0x1800                 # MSTATUS_MPP_MASK (bits 12..11)
    not  t2, t1
    and  t0, t0, t2                 # clear MPP -> U
    csrw mstatus, t0
    csrw mepc, t3

    # Jump to user
    mret
