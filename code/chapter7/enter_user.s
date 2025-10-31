    .section .text
    .globl enter_user
    .type  enter_user, @function

# void enter_user(void *entry, uintptr_t gp_val,
#                 uintptr_t user_sp, size_t arg_size, uintptr_t ksp)
# a0=entry, a1=gp_val, a2=user_sp, a3=arg_size, a4=ksp
enter_user:
    # --- Save incoming args into temps ---
    mv   s2, a0           # user entry PC
    mv   s3, a1           # user gp
    mv   s4, a2           # user sp
    mv   s5, a3           # arg size
    mv   s6, a4           # kernel sp (for sscratch)

    # Program per-process kernel stack for future traps
    csrw sscratch, s6

    # Disable interrupts during handoff (SIE bit in sstatus)
    csrrc t0, sstatus, (1 << 1)

    # --- Clear caller-saved regs (for a clean user context) ---
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
    mv   a0, s4           # arg_buf pointer (same convention)
    mv   a1, s5           # arg_buf size

    # --- Switch privilege to U-mode ---
    csrr  t0, sstatus
    li    t1, (1 << 8)    # SSTATUS_SPP bit
    not   t2, t1
    and   t0, t0, t2      # clear SPP -> next = U
    csrw  sstatus, t0

    # Program user PC
    csrw  sepc, s2

    # --- Enter user mode ---
    sret
