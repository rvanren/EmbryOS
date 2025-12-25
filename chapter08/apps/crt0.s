.globl _start
_start:
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    mv      a0, sp
    call    _crt_init
