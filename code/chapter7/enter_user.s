.section .text
.globl enter_user
.type enter_user, @function

# void enter_user(void *entry, void *ustack);
#   a0 = user entry point
#   a1 = user stack pointer
enter_user:
    # Set the S-mode exception PC to the user entry
    csrw sepc, a0

    # Load user stack pointer
    mv   sp, a1

    # Clear SPP (bit 8) so sret enters U-mode
    csrr t0, sstatus
    li   t1, ~(1 << 8)
    and  t0, t0, t1
    csrw sstatus, t0

    # Disable interrupts in user mode (optional safety)
    csrr t0, sstatus
    li   t1, ~(1 << 5)         # clear SPIE bit
    and  t0, t0, t1
    csrw sstatus, t0

    # Jump into user mode
    sret
