    .section .text.enter
    .global _start

_start:
    csrr a0, mhartid      # read core ID into a0
    beq a0, x0, _end      # if hart 0, jump to _end and spin
    li sp, 0x80400000     # set the stack pointer to top of stack

    # pmp0: 4 MiB region from 0x80000000–0x803FFFFF
    li t0, 0x2003FFFF
    csrw pmpaddr0, t0

    # 0x1F = R/W/X, A=NAPOT (11), L=0
    li t0, 0x1F
    csrw pmpcfg0, t0

    call main             # call C main() code
_end:
    j _end                # just loop for ever
