#include <stddef.h>
#include "frame.h"
#include "process.h"
#include "die.h"
#include "vm.h"

void proc_check_legal(struct pcb *self, uintptr_t start, int size) {
    if (size == 0) return;
    uintptr_t end = start + size - 1;
    if (end < start) die("bad system call size");
    if (start < VM_START || end >= VM_END) die("bad system call address");
}
