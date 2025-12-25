    .section .text
    .align 2

    .global sbi_putchar, sbi_set_timer, mtime_get, sbi_hart_start

# void sbi_putchar(int ch);
#   a0 = ch
sbi_putchar:
    andi    a0, a0, 0xFF         # only low 8 bits matter
    li      a7, 1                # SBI_CONSOLE_PUTCHAR
    ecall
    ret

# void sbi_set_timer(uint64_t next_time);
# RV32: a0=low32, a1=hi32, a6=0, a7="TIME"
# RV64: a0=full64, a6=0, a7="TIME"
# void sbi_set_timer(uint64_t next_time);
# RV32: a0=low32, a1=hi32
# RV64: a0=full64
sbi_set_timer:
    # RV32: ABI already placed low32 in a0 and high32 in a1
    # RV64: ABI already placed full 64-bit value in a0
    li      a6, 0
    li      a7, 0x54494D45       # "TIME"
    ecall
    ret

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

# sword_t sbi_hart_start(uword_t hartid,
#                        uword_t start_addr,
#                        uword_t opaque)
#   a0 = hartid
#   a1 = start address
#   a2 = opaque
#   a3 = 0
#   a6 = 0 (function ID)
#   a7 = "HSM"
# Returns value in a0.
sbi_hart_start:
    mv      a3, zero
    li      a6, 0
    li      a7, 0x48534D         # "HSM"
    ecall
    ret
