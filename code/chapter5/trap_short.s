    .section .text
    .global _trap_handler

/* Single point of entry and exit in the OS */
_trap_handler:
    addi sp, sp, -128

    /* Save Registers */
    sw a0,  0(sp)
    sw a1,  4(sp)
    sw a2,  8(sp)
    ...

    /* add C handler here */
    jal software_trap_handler

    /* Restore Regs */
    lw a0,  0(sp)
    lw a1,  4(sp)
    lw a2,  8(sp)
    ...

    addi sp, sp, 128

    /* Return From Interrupt */
    mret
