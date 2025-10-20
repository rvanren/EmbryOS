    .section .text
    .global _start
_start:
    call main
    li a7, 0          # SYS_EXIT = 0
    ecall             # trap into the kernel
