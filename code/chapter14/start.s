    .section .text.enter
    .globl _start

# ------------------------------------------------------------
# Multicore entry
# Each hart computes its own stack from linker symbols.
# Hart 0 performs global init, others wait until signaled.
# ------------------------------------------------------------

_start:
    # Read current hart ID (unique per core)
    csrr    a0, mhartid              # a0 = hartid

    # Load linker symbols for stack region
    la      sp, stacks_end           # top of all stacks
    la      t0, STACK_SIZE
    mul     t1, a0, t0
    sub     sp, sp, t1               # sp = top - hartid * STACK_SIZE

    # Optional: record per-hart pointer in tp for fast access
    # (if you have a per-CPU struct array called cpus[])
    # la   t2, cpus
    # slli t3, a0, 3                 # sizeof(struct cpu_state) = 8?
    # add  t2, t2, t3
    # mv   tp, t2

    # Identify primary (hart 0) vs secondary
    bnez    a0, secondary

primary:
    # Hart 0 performs all one-time init
    call    kinit_primary            # sets up MMU, PMP, devices, etc.

    # Release secondaries
    la      t0, boot_ok
    li      t1, 1
    sw      t1, 0(t0)
    fence   rw, rw                   # ensure visible to all harts

    j       common_start

secondary:
    # Spin until hart 0 finished init
1:
    la      t0, boot_ok
    lw      t1, 0(t0)
    beqz    t1, 1b                   # wait until boot_ok == 1
    fence   rw, rw                   # memory barrier

common_start:
    # Call per-hart C entry: kstart(hartid)
    mv      a0, a0                   # already hartid
    call    kstart

halt:
    wfi                                # idle loop
    j       halt
