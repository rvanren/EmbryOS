#include "syscall.h"
#include "process.h"
#include "stdio.h"
#include "sched.h"

// Single system call: write a character to the screen at (row, col, fg, bg)
void syscall_handler(void) {
    register int num asm("a7");
    asm volatile("" : "=r"(num));  // read syscall number
    if (num == SYS_PUT) {
        register int row asm("a0");
        register int col asm("a1");
        register int c   asm("a2");
        register int fg  asm("a3");
        register int bg  asm("a4");
        struct pcb *self = run_queue[proc_current]->next;
        proc_put(self, row, col, c, fg, bg);
    }
}

void syscall_init(void) {
    // Nothing to register yet; future system calls will go here
}
