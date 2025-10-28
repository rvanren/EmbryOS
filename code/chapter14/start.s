    .section .text.enter
    .global _start

_start:
    csrr a0, mhartid      # read core ID into a0
    beq a0, x0, _end      # if hart 0, jump to _end and spin
    la sp, stack_end      # set sp to top of stack region
    call main             # call C main() code
_end:
    wfi                   # go to sleep
    j _end                # go back to sleep
