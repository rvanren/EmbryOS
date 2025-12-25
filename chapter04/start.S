#include "platform.h"

    .section .text.enter
    .global _start

_start:

#ifdef NO_SBI
    la t0, entry_lock          # prevent more than 1 hart from entering
    li t1, 1
    amoswap.w.aq t1, t1, (t0)
    bnez t1, _start

    la   t0, _startS           # S-mode entry point
    csrw mepc, t0              # set return address for mret

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

    csrr a0, mhartid
    li   a1, 0                 # no FDT
    mret                       # enter S-mode, begin EmbryOS
#endif

# Entry point executed in S-mode after OpenSBI hands control to us.
# a0 = hartid, a1 = fdt_ptr (both passed by OpenSBI)
_startS:
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

    .section .data
    .align 4
entry_lock:       .word 0

# Stack for embryos_main()
    .section .bss
    .align 16
    .space 4096
    .global stack_end
stack_end:
