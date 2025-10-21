    .section .text
    .globl enter_user
    .type  enter_user,@function
# void enter_user(void *entry, uintptr_t gp_val, uintptr_t user_sp, uintptr_t ksp)
# a0=entry, a1=gp_val, a2=user_sp, a3=ksp
enter_user:
    # Save incoming args into temps BEFORE clobbering aX
    mv   t3, a0           # entry
    mv   t4, a1           # gp_val
    mv   t5, a2           # user_sp
    mv   t6, a3           # ksp

    # Program per-process kernel stack for future traps
    csrw mscratch, t6

    # (Optional but recommended) briefly disable M-mode interrupts during handoff
    # so we can't trap in M while gp=USER:
    csrrc t0, mstatus,  (1 << 3)     # clear MIE

    # Zero all integer regs EXCEPT gp/sp and the temps we still need (t3..t6)
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
    # DO NOT clobber t3,t4,t5,t6 (they hold entry,gp,sp,ksp)

    # Load USER gp/sp from saved temps
    mv   gp, t4
    mv   sp, t5

    # Set return target and privilege: MPP=U, mepc=entry
    csrr t0, mstatus
    li   t1, 0x1800                 # MSTATUS_MPP_MASK (bits 12..11)
    not  t2, t1
    and  t0, t0, t2                 # clear MPP -> U
    csrw mstatus, t0
    csrw mepc, t3

    # mret to user
    mret

    .globl user_setup
user_setup:
    # pmp0: 4 MiB region from 0x80000000-0x803FFFFF
    li t0, 0x2003FFFF
    csrw pmpaddr0, t0

    # 0x1F = R/W/X, A=NAPOT (11), L=0
    li t0, 0x1F
    csrw pmpcfg0, t0
    ret
