#ifndef FRAME_H
#define FRAME_H

#define PAGE_SIZE      4096

struct page {
    char bytes[PAGE_SIZE];
};

struct free_frame {
    int next;       // next free frame
};

extern struct page frames[];

void frame_init();
int frame_alloc();
void frame_release(int f);

#endif // FRAME_H
