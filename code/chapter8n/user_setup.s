    .globl user_setup
user_setup:
    # pmp0: 4 MiB region from 0x80000000-0x803FFFFF
    li t0, 0x2003FFFF
    csrw pmpaddr0, t0

    # 0x1F = R/W/X, A=NAPOT (11), L=0
    li t0, 0x1F
    csrw pmpcfg0, t0
    ret
