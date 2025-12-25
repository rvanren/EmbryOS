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

    # void user_spawn(int app, int row, char col, int wd, int ht,
    #                 const void *args, int sz)
user_spawn:
    li      a7, SYS_SPAWN
    ecall
    ret

    # void user_put(int row, int col, cell_t cell)
user_put:
    li      a7, SYS_PUT
    ecall
    ret

    # int user_get(int block)
user_get:
    li      a7, SYS_GET
    ecall
    ret

    # int user_create(void)
user_create:
    li      a7, SYS_CREATE
    ecall
    ret

    # int user_read(int file, int off, void* dst, int n)
user_read:
    li      a7, SYS_READ
    ecall
    ret

    # int user_write(int file, int off, const void* src, int n)
user_write:
    li      a7, SYS_WRITE
    ecall
    ret

    # int user_size(int file)
user_size:
    li      a7, SYS_SIZE
    ecall
    ret

    # void user_delete(int file)
user_delete:
    li      a7, SYS_DELETE
    ecall
    ret
