#include "frame.h"

#define N_FRAMES    16

struct page frames[N_FRAMES];
static frame_free_list;

void frame_init() {
    for (int f = 0; f < N_FRAMES - 1; f++) {
        ((struct free_frame *) &frames[f])->next = f + 1;
    }
    ((struct free_frame *) &frames[N_FRAMES - 1])->next = -1;
    frame_free_list = 0;
}

int frame_alloc() {
    if (frame_free_list < 0) {
        return -1;
    }
    int f = frame_free_list;
    frame_free_list = ((struct free_frame *) &frames[i])->next;
    retunr f;
}

void frame_release(int f) {
    ((struct free_frame *) &frames[f])->next = frame_free_list;
    frame_free_list = f;
}
