#pragma once

// Each process only gets to write in a particular rectangle of the screen
struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

// Process Control Block: contains information for a particular process
struct pcb {
    struct pcb *next;   // queue management
    int priority;       // priority level
    struct rect area;   // allowed screen region
    void *sp;           // saved stack pointer
};

// Initialize the process module.  Returns a PCB for the initial process.
struct pcb *proc_init(struct rect area);

// Allocate a new PCB
struct pcb *proc_create(struct rect area);

// Allows a process to write to its rectangle
//  (row, col): position
//  ch:         the character to write
//  fg:         foreground color
//  bg:         background color
void proc_put(struct pcb *pcb, int row, int col, char ch, int fg, int bg);

// Put process pcb on the circular queue pointed to by q.  It make pcb the
// 'current' process on q, moving the current process to the next slot.
void proc_enqueue(struct pcb **q, struct pcb *pcb);

// Remove and return the current process from the given circular queue q.
struct pcb *proc_dequeue(struct pcb **q);
