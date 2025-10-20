    .section .text
    .global ctx_switch, ctx_start

# void ctx_switch(void **old_sp, void *new_sp)
    ...

# void ctx_start(void **save_sp, void *new_sp, void (*entry)(void))
# Switch to a new stack and start running entry() in user mode.

    .section .text
    .globl ctx_start
ctx_start:
    addi sp, sp, -64
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    ...
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

    .globl ctx_user_setup
ctx_user_setup:
    # pmp0: 4 MiB region from 0x80000000-0x803FFFFF
    li t0, 0x2003FFFF
    csrw pmpaddr0, t0

    # 0x1F = R/W/X, A=NAPOT (11), L=0
    li t0, 0x1F
    csrw pmpcfg0, t0
    ret
