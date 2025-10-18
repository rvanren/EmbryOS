#include "frame.h"
#include "stdio.h"

#define N_FRAMES    16

struct page frames[N_FRAMES];
static int frame_free_list;

void frame_init() {
    for (int f = 0; f < N_FRAMES - 1; f++) {
        FRAME(struct free_frame, f)->next = f + 1;
    }
    FRAME(struct free_frame, N_FRAMES - 1)->next = -1;
    frame_free_list = 0;
}

int frame_alloc() {
    if (frame_free_list < 0) {
        printf("OUT OF FRAMES\n");
        for (;;) ;
    }
    int f = frame_free_list;
    frame_free_list = FRAME(struct free_frame, f)->next;
    return f;
}

void frame_release(int f) {
    FRAME(struct free_frame, f)->next = frame_free_list;
    frame_free_list = f;
}
