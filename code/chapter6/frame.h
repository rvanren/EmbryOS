#pragma once

// Frames are pages of physical memory.  They are the unit of dynamic memory allocation
// in the EmbryOS kernel.

#define PAGE_SIZE      4096

// Cast a frame f to a pointer of the given type t
#define FRAME(t, f)     ((t *) &frames[f])

struct page {
    char bytes[PAGE_SIZE];
};

// This is type of unallocated frames.  They point to the next unallocated frame.
struct free_frame {
    int next;       // -1 means there is no other unallocated frame
};

// This is the base address of the frames.
extern struct page frames[];

// Initialize the frame module
void frame_init();

// Allocate a frame
int frame_alloc();

// Release a frame
void frame_release(int f);
