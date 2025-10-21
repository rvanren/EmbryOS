    .section .text
    .global ctx_switch, ctx_start

# void ctx_switch(void **old_sp, void *new_sp)
# ------------------------------------------------------------
# Saves current context into *old_sp, switches to new_sp, and
# restores that context. Returns into the restored context.
# old_sp = a0; new_sp = a1
ctx_switch:
    addi sp, sp, -64
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10,44(sp)
    sw s11,48(sp)
    sw ra, 52(sp)
    sw sp, 0(a0)
    mv sp, a1
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    lw s7, 32(sp)
    lw s8, 36(sp)
    lw s9, 40(sp)
    lw s10, 44(sp)
    lw s11, 48(sp)
    lw ra, 52(sp)
    addi sp, sp, 64
    ret

# void ctx_start(void **save_sp, void *new_sp, void (*entry)(void))
# ------------------------------------------------------------
# Saves current stack pointer, switches to new stack, and calls
# the entry function.
# old_sp = a0; new_sp = a1; entry = a2
ctx_start:
    addi sp, sp, -64
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)
    sw s9, 40(sp)
    sw s10, 44(sp)
    sw s11, 48(sp)
    sw ra, 52(sp)
    sw sp, 0(a0)
    mv sp, a1
    jalr a2
