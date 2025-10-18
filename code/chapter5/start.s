    .section .text.enter
    .global _start

_start:
    csrr a0, mhartid      # read core ID into a0
    beq a0, x0, _end      # if hart 0, jump to _end and spin
    li sp, 0x80400000     # set the stack pointer to top of stack

    li t0, 0x80000000 >> 2      # address / 4
    csrw pmpaddr0, t0
    li t0, 0x1F                 # TOR, R/W/X for U
    csrw pmpcfg0, t0

    call main             # call C main() code
_end:
    j _end                # just loop for ever
