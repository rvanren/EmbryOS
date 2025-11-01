#include <stddef.h>
#include "frame.h"
#include "process.h"
#include "die.h"

void proc_check_legal(struct pcb *self, uintptr_t start, int size) {
    if (size == 0) return;
    uintptr_t end = start + size - 1;
    if (end < start) die("bad system call size");
    if (start < PAGE_SIZE || end >= 0x400000) die("bad system call address");
}
