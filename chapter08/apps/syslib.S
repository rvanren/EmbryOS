#include "syscode.h"

    .section .text
    .align 2
    .globl user_exit, user_yield, user_spawn, user_put, user_get
    .globl user_create, user_read, user_write, user_size, user_delete

    # void user_exit(void);
user_exit:
    li      a7, SYS_EXIT
    ecall
    ret

    # void user_yield(void);
user_yield:
    li      a7, SYS_YIELD
    ecall
    ret

    # void user_spawn(int app, int col, int row, int wd, int ht,
    #                 const void *args, int sz)
user_spawn:
    li      a7, SYS_SPAWN
    ecall
    ret

    # void user_put(int col, int row, cell_t cell)
user_put:
    li      a7, SYS_PUT
    ecall
    ret

    # int user_get(int block)
user_get:
    li      a7, SYS_GET
    ecall
    ret
