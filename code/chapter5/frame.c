#include <stdint.h>
#include "frame.h"

extern struct page __frames_end[];
static int free_head = 0;

void frame_init(void) {
    int nframes = __frames_end - frames;
    for (int i = 0; i < nframes - 1; i++)
        FRAME(struct free_frame, i)->next = i + 1;
    FRAME(struct free_frame, nframes - 1)->next = -1;
}

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
