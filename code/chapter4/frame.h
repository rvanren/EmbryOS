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

#endif // FRAME_H
