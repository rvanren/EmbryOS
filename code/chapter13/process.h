#pragma once

#include <stdint.h>
#include "syscall.h"

#define KBD_BUF_SIZE 64

// Each process only gets to write in a particular rectangle of the screen
struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

// Process Control Block: contains information for a particular process
struct pcb {
    struct pcb *next;   // queue management
    int priority;       // priority level
    int executable;     // file containing executable
    struct rect area;   // allowed screen region
    cell_t cf, cu;      // focused cursor, unfocused cursor
    char kbd_buf[KBD_BUF_SIZE];     // circular keyboard buffer
    int kbd_tail, kbd_size;         // meta data for kbd buffer
    int kbd_row, kbd_col;           // cursor position
    int kbd_waiting;    // waiting for input
    void *args;         // arguments buffer
    int size;           // size of arguments buffer
    void *sp;           // saved stack pointer
    char *base, *stack; // user space frames
};

// Initialize the process module.  Returns a PCB for the initial process.
struct pcb *proc_init(struct rect area);

// Allocate a new PCB
struct pcb *proc_create(int file, struct rect area, void *args, int size);

// Allows a process to write to its rectangle
//  (row, col): position
//  cell:       the character to write (incl. fg/bg color)
void proc_put(struct pcb *pcb, int row, int col, cell_t cell);

// Put process pcb on the circular queue pointed to by q.  It make pcb the
// 'current' process on q, moving the current process to the next slot.
void proc_enqueue(struct pcb **q, struct pcb *pcb);

// Remove and return the current process from the given circular queue q.
struct pcb *proc_dequeue(struct pcb **q);

// Terminate the currently running process (never returns).
void proc_exit(void);

// Free all processes previously marked for exit.
void proc_reap_zombies(void);

// Check if the given address range is legal
void proc_check_legal(struct pcb *self, uintptr_t start, int size);
