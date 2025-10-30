    .section .text
    .globl _start

# ----------------------------------------------------------------------
#  Entry point executed in S-mode after OpenSBI hands control to us.
#  a0 = hartid,  a1 = dtb_ptr  (passed by OpenSBI)
# ----------------------------------------------------------------------
_start:
    la   sp, stack_end
    call main                 # jump into C code
1:  wfi
    j    1b

# ----------------------------------------------------------------------
#  Simple 8 KiB stack
# ----------------------------------------------------------------------
    .section .bss
    .align 4
    .space 8192
    .global stack_end
stack_end:
