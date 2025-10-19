    .section .text
    .global _start
_start:
    la sp, 0x80800000     # initialize stack
    call main
1:  j 1b
