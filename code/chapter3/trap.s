    .section .text
    .global _trap_handler

/* Single point of entry and exit in the OS */
_trap_handler:
    addi sp, sp, -128

    /* Save Registers */
    sw a0,  0(sp)
    sw a1,  4(sp)
    sw a2,  8(sp)
    sw a3,  12(sp)
    sw a4,  16(sp)
    sw a5,  20(sp)
    sw a6,  24(sp)
    sw a7,  28(sp)
    sw t0,  32(sp)
    sw t1,  36(sp)
    sw t2,  40(sp)
    sw t3,  44(sp)
    sw t4,  48(sp)
    sw t5,  52(sp)
    sw t6,  56(sp)
    sw s0,  60(sp)
    sw s1,  64(sp)
    sw s2,  68(sp)
    sw s3,  72(sp)
    sw s4,  76(sp)
    sw s5,  80(sp)
    sw s6,  84(sp)
    sw s7,  88(sp)
    sw s8,  92(sp)
    sw s9,  96(sp)
    sw s10, 100(sp)
    sw s11, 104(sp)
    sw ra,  108(sp)
    sw gp,  112(sp)
    sw tp,  116(sp)

    /* add C handler here */
    jal software_trap_handler

    /* Restore Regs */
    lw a0,  0(sp)
    lw a1,  4(sp)
    lw a2,  8(sp)
    lw a3,  12(sp)
    lw a4,  16(sp)
    lw a5,  20(sp)
    lw a6,  24(sp)
    lw a7,  28(sp)
    lw t0,  32(sp)
    lw t1,  36(sp)
    lw t2,  40(sp)
    lw t3,  44(sp)
    lw t4,  48(sp)
    lw t5,  52(sp)
    lw t6,  56(sp)
    lw s0,  60(sp)
    lw s1,  64(sp)
    lw s2,  68(sp)
    lw s3,  72(sp)
    lw s4,  76(sp)
    lw s5,  80(sp)
    lw s6,  84(sp)
    lw s7,  88(sp)
    lw s8,  92(sp)
    lw s9,  96(sp)
    lw s10, 100(sp)
    lw s11, 104(sp)
    lw ra,  108(sp)
    lw gp,  112(sp)
    lw tp,  116(sp)

    addi sp, sp, 128

    /* Return From Interrupt */
    mret
