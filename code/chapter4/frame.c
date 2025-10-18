#include <stdint.h>
#include "frame.h"

extern char __frames_start[];
extern char __frames_end[];

struct page *frames = (struct page *)__frames_start;
int nframes;

void frame_init(void) {
    nframes = ((uintptr_t)__frames_end - (uintptr_t)__frames_start) / PAGE_SIZE;

    for (int i = 0; i < nframes - 1; i++)
        FRAME(struct free_frame, i)->next = i + 1;
    FRAME(struct free_frame, nframes - 1)->next = -1;
}

static int free_head = 0;

int frame_alloc(void) {
    if (free_head < 0) return -1;
    int f = free_head;
    free_head = FRAME(struct free_frame, f)->next;
    return f;
}

void frame_release(int f) {
    FRAME(struct free_frame, f)->next = free_head;
    free_head = f;
}
