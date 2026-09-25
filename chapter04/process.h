#pragma once

#include <stdint.h>
#include "syscall.h"
#include "hart.h"

#define KBD_BUF_SIZE 64

// Each process only gets to write in a particular rectangle of the screen
struct rect {
    int col, row;   // top-left corner on global screen
    int wd, ht;     // width and height
};

enum proc_state { RUNNABLE, RUNNING, ZOMBIE };

// Process Control Block: contains information for a particular process
struct pcb {
    struct pcb *next;     // queue management
    int executable;       // file containing executable
    struct rect area;     // allowed screen region
    char *args; int size; // arguments buffer
    void *sp;             // kernel sp saved on context switch
    enum proc_state state;
    uint64_t wait_start;
    struct hart *hart;    // the hart the process is running on
};

// Allocate a new PCB
struct pcb *proc_create(struct hart *hart, int file, struct rect area, void *args, int size);

// Allows a process to write to its rectangle.  (row, col): position.
//  cell: the character to write (incl. fg/bg color)
void proc_put(struct pcb *pcb, int row, int col, cell_t cell);

// Release the given PCB
void proc_release(struct pcb *pcb);
