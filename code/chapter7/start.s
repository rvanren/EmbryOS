    .section .text.enter
    .global _start

.align 2
_start:
    csrr a0, mhartid      # read core ID into a0
    beq a0, x0, _end      # if hart 0, jump to _end and spin
    li sp, 0x80400000     # set the stack pointer to top of stack

    call main             # call C main() code
_end:
    j _end                # just loop for ever


# Call early in M-mode, after setting mtvec
# Opens PMP so M-mode can execute/fetch anywhere (no lock bit).
.global pmp_open_all
pmp_open_all:
    # Clear all PMP configs (entries 0..15)
    csrw pmpcfg0, zero
    csrw pmpcfg1, zero
    csrw pmpcfg2, zero
    csrw pmpcfg3, zero

    # Clear all PMP addresses (entries 0..15)
    csrw pmpaddr0,  zero
    csrw pmpaddr1,  zero
    csrw pmpaddr2,  zero
    csrw pmpaddr3,  zero
    csrw pmpaddr4,  zero
    csrw pmpaddr5,  zero
    csrw pmpaddr6,  zero
    csrw pmpaddr7,  zero
    csrw pmpaddr8,  zero
    csrw pmpaddr9,  zero
    csrw pmpaddr10, zero
    csrw pmpaddr11, zero
    csrw pmpaddr12, zero
    csrw pmpaddr13, zero
    csrw pmpaddr14, zero
    csrw pmpaddr15, zero

    # Install a single allow-all NAPOT region: pmpaddr0 = all-ones >> 2
    li   t0, -1
    srli t0, t0, 2
    csrw pmpaddr0, t0

    # pmpcfg0 entry 0: R|W|X with A=NAPOT (0b11<<3 = 0x18), no L-bit → 0x1f
    li   t0, 0x1f
    csrw pmpcfg0, t0
    ret
