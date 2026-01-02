#pragma once

#include <stdint.h>
#include "syscall.h"
#include "trap.h"
#include "hart.h"
#include "selfie.h"

#define KBD_BUF_SIZE 64

// Each process only gets to write in a particular rectangle of the screen
struct rect {
    int col, row;   // top-left corner on global screen
    int wd, ht;     // width and height
};

// Process Control Block: contains information for a particular process
struct pcb {
    struct pcb *next, *io_next;     // queue management
    int executable;                 // file containing executable
    struct rect area;               // allowed screen region
    int16_t kbd_buf[KBD_BUF_SIZE];  // circular keyboard buffer
    int kbd_head, kbd_size;         // meta data for kbd buffer
    unsigned int kbd_waiting : 1;   // waiting for input
    unsigned int kbd_warm : 1;      // first get() gets focus
    char *args; int size;           // arguments buffer
    void *sp;             // kernel sp saved on context switch
    char *base;           // page table
    struct selfie selfie; // selfie process state
    struct hart *hart;    // the hart the process is running on
};

// Allocate a new PCB
struct pcb *proc_create(struct hart *hart, int file, struct rect area, void *args, int size);

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
