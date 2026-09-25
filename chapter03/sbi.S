    .section .text
    .align 2

    .global sbi_putchar, sbi_set_timer

# void sbi_putchar(int ch);
#   a0 = ch
sbi_putchar:
    andi    a0, a0, 0xFF         # only low 8 bits matter
    li      a7, 1                # SBI_CONSOLE_PUTCHAR
    ecall
    ret

    .section .text
    .globl mtime_get
    .align  2

# uint64_t mtime_get(void)
mtime_get:
#if __riscv_xlen == 32
    # Loop to get a stable 64-bit time
1:
    csrr    t1, timeh        # hi
    csrr    t0, time         # lo
    csrr    t2, timeh        # hi2
    bne     t1, t2, 1b       # retry if rollover occurred

    # Return as 64-bit in a0 (low) and a1 (high), per RV32 ABI
    mv      a0, t0           # low 32 bits
    mv      a1, t1           # high 32 bits
    ret
#endif

#if __riscv_xlen == 64
    csrr    a0, time         # returns full 64-bit time
    ret
#endif
