#include <stddef.h>
#include "frame.h"
#include "process.h"
#include "stdio.h"

void proc_check_legal(struct pcb *self, uintptr_t start, int size) {
    if (size == 0) return;
    uintptr_t end = start + size - 1;
    if (end < start) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        printf("bad system call size<");
        proc_exit();
    }
    uintptr_t base_lo  = (uintptr_t) self->base;
    uintptr_t base_hi  = base_lo  + PAGE_SIZE - 1;
    uintptr_t stack_lo = (uintptr_t) self->stack;
    uintptr_t stack_hi = stack_lo + PAGE_SIZE - 1;
    if (!((start >= base_lo && end <= base_hi) ||
                    (start >= stack_lo && end <= stack_hi))) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        printf("bad system call address<");
        proc_exit();
    }
}
