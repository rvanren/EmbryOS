# chapter4/trap.s
# Minimal machine-mode trap entry for RV32I/M.
# Saves caller-saved + callee-saved regs sufficient to call a C handler.

    .section .text.enter
    .globl trap_entry
    .align 2

# Upon entry:
#  - mcause contains cause (bit31=interrupt, low bits=cause code)
#  - mepc is the return PC
# We:
#  - make a small stack frame on the current stack
#  - save caller-saved regs that C may clobber
#  - pass (mcause, mepc) to C: trap_handler(mcause, mepc)
#  - restore regs and mret

trap_entry:
    addi    sp, sp, - (4*13)        # save 13 regs: ra,a0-a7,t0-t2,t3?
    sw      ra,   0(sp)
    sw      a0,   4(sp)
    sw      a1,   8(sp)
    sw      a2,  12(sp)
    sw      a3,  16(sp)
    sw      a4,  20(sp)
    sw      a5,  24(sp)
    sw      a6,  28(sp)
    sw      a7,  32(sp)
    sw      t0,  36(sp)
    sw      t1,  40(sp)
    sw      t2,  44(sp)
    sw      t3,  48(sp)

    # Read mcause -> a0, mepc -> a1
    csrr    a0, mcause
    csrr    a1, mepc

    # Call into C handler
    la      t0, trap_handler
    jalr    ra, t0, 0

    # Restore regs
    lw      ra,   0(sp)
    lw      a0,   4(sp)
    lw      a1,   8(sp)
    lw      a2,  12(sp)
    lw      a3,  16(sp)
    lw      a4,  20(sp)
    lw      a5,  24(sp)
    lw      a6,  28(sp)
    lw      a7,  32(sp)
    lw      t0,  36(sp)
    lw      t1,  40(sp)
    lw      t2,  44(sp)
    lw      t3,  48(sp)
    addi    sp, sp, (4*13)

    mret

    .section .text
    .globl trap_handler
    .type  trap_handler, @function
# Simple C-callable dispatcher: handles only machine-timer interrupts.
# void trap_handler(uint32_t mcause, uint32_t mepc);
#  - If mcause indicates machine-timer interrupt (interrupt bit set, code 7),
#    call timer_interrupt().
#  - Otherwise ignore (return to mepc).

trap_handler:
    # a0 = mcause, a1 = mepc
    # Check interrupt bit (bit 31) and code 7
    li      t0, 0x80000000
    and     t1, a0, t0
    beqz    t1, .Ldone             # not an interrupt

    # low bits cause == 7 ? (machine-timer)
    li      t2, 7
    andi    t3, a0, 0xFF
    bne     t3, t2, .Ldone

    # call C timer handler
    la      t4, timer_interrupt
    jalr    ra, t4, 0

.Ldone:
    ret
