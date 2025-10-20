    .section .text
    .globl _start
    .type  _start, @function

_start:
    /* --- Set gp in a PIC-safe way (PC-relative, no relax) --- */
    .option push
    .option norelax
    lla     gp, __global_pointer$     /* lla = auipc+addi (PC-relative), never gp-relative */
    .option pop

    /* --- (Optional) clear .bss in a PIC-safe way --- */
    lla     t0, __bss_start
    lla     t1, __bss_end
1:
    beq     t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:

    /* --- call C entry --- */
    call    main

    /* --- if main returns, exit via syscall --- */
    li      a7, 0              /* SYS_EXIT = 0 */
    ecall                       /* trap to kernel */

    /* no return */

