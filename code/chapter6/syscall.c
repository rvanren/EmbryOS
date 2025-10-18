#include "syscall.h"
#include "process.h"
#include "stdio.h"
#include "sched.h"
#include "kbd.h"

// Single system call: write a character to the screen at (row, col, fg, bg)
void syscall_handler(void) {
    register int num asm("a7");
    asm volatile("" : "=r"(num));  // read syscall number
    switch (num) {
        case SYS_PUT: {
            register int row asm("a0");
            register int col asm("a1");
            register int c   asm("a2");
            register int fg  asm("a3");
            register int bg  asm("a4");
            struct pcb *self = run_queue[proc_current]->next;
            proc_put(self, row, col, c, fg, bg);
            break;
        }
        case SYS_GET: {
            int c = kbd_get();
            asm volatile("mv a0, %0" :: "r"(c));  // return value
            break;
        }
    }
}

void syscall_init(void) {
}
