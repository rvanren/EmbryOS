#pragma once

// Frames are pages of physical memory.  They are the unit of dynamic memory allocation
// in the EmbryOS kernel.

#define PAGE_SHIFT      13
#define PAGE_SIZE       (1 << PAGE_SHIFT)

// Initialize the frame module
void frame_init();

// Allocate a frame
void *frame_alloc();

// Release a frame
void frame_release(void *frame);
