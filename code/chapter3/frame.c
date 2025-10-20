#include <stdint.h>
#include "frame.h"

// This is type of unallocated frames.  They point to the next unallocated frame.
union free_frame {
    union free_frame *next;
    char bytes[PAGE_SIZE];
};

extern union free_frame frames[], __frames_end[];
static union free_frame *free_list = frames;

void frame_init(void) {
    int nframes = __frames_end - frames;
    for (int i = 0; i < nframes - 1; i++)
        frames[i].next = &frames[i + 1];
    frames[nframes - 1].next = 0;
}

void *frame_alloc(void) {
    if (free_list == 0) return 0;
    union free_frame *f = free_list;
    free_list = f->next;
    return f;
}

void frame_release(void *frame) {
    union free_frame *f = frame;
    f->next = free_list;
    free_list = f;
}
