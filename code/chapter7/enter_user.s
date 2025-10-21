    .section .text
    .globl enter_user
    .type  enter_user, @function

# void enter_user(void *entry);
#   a0 = user entry point (e.g., &user_main)
enter_user:
    # Set the program counter for return-from-trap
    csrw mepc, a0

    # Clear MPP bits in mstatus so mret goes to U-mode
    csrr  t0, mstatus
    li    t1, ~(3 << 11)      # clear bits 12–11 (MPP)
    and   t0, t0, t1
    csrw  mstatus, t0

    # Return into user mode on the *current* stack
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
