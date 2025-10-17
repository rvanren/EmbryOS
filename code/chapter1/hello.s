    .section .text.enter
    .global _start

_start:
    csrr a0, mhartid
    beq a0, x0, _end  /* disables core zero */
    li sp, 0x80400000
    call main
_end:
    j _end
