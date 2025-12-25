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
