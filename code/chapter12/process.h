#pragma once

#include <stdint.h>
#include "syscall.h"
#include "trap.h"

#define KBD_BUF_SIZE 64

// Each process only gets to write in a particular rectangle of the screen
struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

// Process Control Block: contains information for a particular process
struct pcb {
    struct pcb *next;     // queue management
    int executable;       // file containing executable
    struct rect area;     // allowed screen region
    cell_t cf, cu;        // focused cursor, unfocused cursor
    char kbd_buf[KBD_BUF_SIZE];     // circular keyboard buffer
    int kbd_tail, kbd_size;         // meta data for kbd buffer
    int kbd_row, kbd_col;           // cursor position
    int kbd_waiting : 1;  // waiting for input
    int kbd_warm : 1;     // first get() gets focus
    void *args; int size; // arguments buffer
    void *sp;             // kernel sp saved on context switch
    char *base, *stack;   // user space frames
};

// Initialize the process module.  Returns a PCB for the initial process.
struct pcb *proc_init(struct rect area);

// Allocate a new PCB
struct pcb *proc_create(int file, struct rect area, void *args, int size);

// Allows a process to write to its rectangle.  (row, col): position.
//  cell: the character to write (incl. fg/bg color)
void proc_put(struct pcb *pcb, int row, int col, cell_t cell);

// Put process pcb on the circular queue pointed to by q.  It make pcb the
// 'current' process on q, moving the current process to the next slot.
void proc_enqueue(struct pcb **q, struct pcb *pcb);

// Remove and return the current process from the given circular queue q.
struct pcb *proc_dequeue(struct pcb **q);

// Check the legality of the given user memory region
void proc_check_legal(struct pcb *self, uintptr_t start, int size);

// Handle a page fault
void proc_pagefault(struct trap_frame *tf);

// Release the given PCB
void proc_release(struct pcb *pcb);
