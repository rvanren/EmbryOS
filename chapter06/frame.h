#pragma once

// Frames are pages of physical memory.  They are the unit of dynamic memory
// allocation in the EmbryOS kernel.

// Initialize the frame module
void frame_init(void *fdt, uintptr_t end);

// Allocate a frame
void *frame_alloc(void);

// Release a frame
void frame_release(void *frame);
