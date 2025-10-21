# ---------------------------------------------------------------------
# void to_user(struct trap_frame *tf);
# Restores all general registers and mret's to tf->mepc.
# ---------------------------------------------------------------------
    .globl to_user
to_user:
    lw ra,  0*4(a0)
    lw sp,  1*4(a0)
    lw gp,  2*4(a0)
    lw tp,  3*4(a0)
    lw t0,  4*4(a0)
    lw t1,  5*4(a0)
    lw t2,  6*4(a0)
    lw s0,  7*4(a0)
    lw s1,  8*4(a0)
    lw a0,  9*4(a0)
    lw a1, 10*4(a0)
    lw a2, 11*4(a0)
    lw a3, 12*4(a0)
    lw a4, 13*4(a0)
    lw a5, 14*4(a0)
    lw a6, 15*4(a0)
    lw a7, 16*4(a0)
    lw s2, 17*4(a0)
    lw s3, 18*4(a0)
    lw s4, 19*4(a0)
    lw s5, 20*4(a0)
    lw s6, 21*4(a0)
    lw s7, 22*4(a0)
    lw s8, 23*4(a0)
    lw s9, 24*4(a0)
    lw s10,25*4(a0)
    lw s11,26*4(a0)
    lw t3, 27*4(a0)
    lw t4, 28*4(a0)
    lw t5, 29*4(a0)
    lw t6, 30*4(a0)
    lw t1, 31*4(a0)       # mepc
    csrw mepc, t1
    lw t1, 32*4(a0)       # mstatus
    csrw mstatus, t1
    mret
