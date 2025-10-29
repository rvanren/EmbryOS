#include <stdint.h>
#include <string.h>
#include "bd_simple.h"
#include "die.h"

int simple_alloc(void *st) {
    struct simple_state *s = st;
    int inode = simple_alloc_block(s);
    if (inode == 0) return 0;
    static struct inode_block ib = { 0 };
    s->lower->write(s->lower->state, s->inode_below, inode, &ib);
    return inode;
}

int simple_size(void *st, int inode) {
    struct simple_state *s = st;
    static struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    for (int i = N_POINTERS - 1; i >= 0; i--)
        if (ib.blocks[i] != 0) return i + 1;
    return 0;
}

void simple_read(void *st, int inode, int blk, void *dst) {
    struct simple_state *s = st;
    static struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    int b = blk < N_POINTERS ? ib.blocks[blk] : 0;
    if (b == 0) memset(dst, 0, BLOCK_SIZE);
    else s->lower->read(s->lower->state, s->inode_below, b, dst);
}

void simple_write(void *st, int inode, int blk, const void *src) {
    struct simple_state *s = st;
    if ((unsigned) blk >= N_POINTERS) return;
    static struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    if (ib.blocks[blk] == 0) {
        int nb = simple_alloc_block(s);
        if (nb == 0) die("disk full");
        ib.blocks[blk] = nb;
        s->lower->write(s->lower->state, s->inode_below, inode, &ib);
    }
    s->lower->write(s->lower->state, s->inode_below, ib.blocks[blk], src);
}

void simple_free(void *st, int inode) {
    struct simple_state *s = st;
    static struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    for (int i = 0; i < N_POINTERS; i++)
        if (ib.blocks[i] != 0) simple_free_block(s, ib.blocks[i]);
    simple_free_block(s, inode);
}
