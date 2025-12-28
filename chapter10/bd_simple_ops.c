#include "embryos.h"

int simple_alloc(void *st) {
    struct simple_state *s = st;
    int inode = simple_alloc_block(s);
    L1(L_NORM, L_SIMPLE_ALLOC, inode);
    if (inode == 0) return 0;
    s->lower->write(s->lower->state, s->inode_below, inode, &bd_null_block);
    return inode;
}

int simple_size(void *st, int inode) {
    L1(L_NORM, L_SIMPLE_SIZE, inode);
    return SIMPLE_N_POINTERS;
}

void simple_read(void *st, int inode, int blk, void *dst) {
    L3(L_NORM, L_SIMPLE_READ, inode, blk, (uintptr_t) dst);
    if (blk < 0 || blk >= SIMPLE_N_POINTERS) die("simple_read: bad offset");
    struct simple_state *s = st;
    struct simple_inode_block *ib = (struct simple_inode_block *) bd_alloc();
    s->lower->read(s->lower->state, s->inode_below, inode, ib);
    int b = ib->blocks[blk];
    if (b == 0) memset(dst, 0, BLOCK_SIZE);
    else s->lower->read(s->lower->state, s->inode_below, b, dst);
    bd_free((struct block *) ib);
}

void simple_write(void *st, int inode, int blk, const void *src) {
    L3(L_NORM, L_SIMPLE_WRITE, inode, blk, (uintptr_t) src);
    if (blk < 0 || blk >= SIMPLE_N_POINTERS) die("simple_write: bad offset");
    struct simple_state *s = st;
    struct simple_inode_block *ib = (struct simple_inode_block *) bd_alloc();
    s->lower->read(s->lower->state, s->inode_below, inode, ib);
    if (ib->blocks[blk] == 0) {
        ib->blocks[blk] = simple_alloc_block(s);
        s->lower->write(s->lower->state, s->inode_below, inode, ib);
    }
    s->lower->write(s->lower->state, s->inode_below, ib->blocks[blk], src);
    bd_free((struct block *) ib);
}

void simple_free(void *st, int inode) {
    L1(L_NORM, L_SIMPLE_FREE, inode);
    struct simple_state *s = st;
    struct simple_inode_block *ib = (struct simple_inode_block *) bd_alloc();
    s->lower->read(s->lower->state, s->inode_below, inode, ib);
    for (int i = 0; i < SIMPLE_N_POINTERS; i++)
        if (ib->blocks[i] != 0) simple_free_block(s, ib->blocks[i]);
    bd_free((struct block *) ib);
    simple_free_block(s, inode);
}
