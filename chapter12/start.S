#include "platform.h"

#if __riscv_xlen == 64
    #define REG_L   ld
    #define REG_S   sd
    #define REG_W   .dword
    #define PTRSZ   8
#else
    #define REG_L   lw
    #define REG_S   sw
    #define REG_W   .word
    #define PTRSZ   4
#endif

    .section .text.enter
    .global _start, startS

_start:

# First ensure S-mode
#ifdef NO_SBI
    csrr a0, mhartid
    li   a1, 0                 # no FDT

    csrr t1, mstatus
    li   t2, ~(3 << 11)        # mask to clear MPP bits (bits 12:11)
    and  t1, t1, t2
    li   t2, (1 << 11)         # set MPP = S-mode (01)
    or   t1, t1, t2
    csrw mstatus, t1

    li  t0, (~0)               # pmpaddr0 = all 0xFFFFFFFF_FFFFFFFF region
    csrw pmpaddr0, t0
    li  t0, 0xF                # TOR mode, R/W/X permissions
    csrw pmpcfg0, t0

    # Delegate page faults and ECALL from U-mode
    li   t0, (1 << 12) | (1 << 13) | (1 << 15) | (1 << 8)
    csrw medeleg, t0

    # Delegate supervisor interrupts
    li   t0, (1 << 1) | (1 << 5) | (1 << 9)
    csrw mideleg, t0

    la   t0, startS            # S-mode entry point
    csrw mepc, t0              # set return address for mret
    mret                       # enter S-mode, begin EmbryOS
#endif

# Entry point executed in S-mode. a0 = hartid, a1 = fdt_ptr
# Grab the big kernel lock
startS:
    la t0, bkl                 # prevent more than 1 hart from entering
    li t1, 1
    amoswap.w.aq t1, t1, (t0)
    bnez t1, startS

# See if we're the first
    la    t0, hart_next        # t0 = &hart_next
    REG_L tp, 0(t0)            # tp = hart_next (pointer-sized)
    bnez tp, hart_entry        # if (hart_next != 0) goto hart_entry

# This is the first hart
first:
    li   tp, 0
    la   t0, __bss_start
    la   t1, __bss_end

    # Clear bss (uninitialized variables) section
    li   t2, 0
1:  bgeu t0, t1, 2f
    sw   t2, 0(t0)
    addi t0, t0, 4
    j    1b

    # Call embryos_main() with valid stack
2:  la   sp, stack_end

    .option push
    .option norelax
    la   gp, __global_pointer$
    .option pop

    call embryos_main                 # jump into C code

    .globl hart_entry

# Entry point of all harts except the first
hart_entry:
    li   t0, PAGE_SIZE                # t0 = PAGE_SIZE
    add  sp, tp, t0                   # sp = tp + PAGE_SIZE

    .option push
    .option norelax
    la   gp, __global_pointer$
    .option pop

    call hart_main

    .section .data
    .align 4
    .global bkl
bkl:
    .word 0                 # spinlock is intentionally 32-bit

    .align PTRSZ
    .global hart_next
hart_next:
    REG_W 0                 # pointer-sized storage

# Stack for embryos_main()
    .section .bss
    .align 16
    .space 4096
    .global stack_end
stack_end:
