#include <stdint.h>
#include <string.h>
#include "bd.h"

#define N_POINTERS (BLOCK_SIZE / 4)
struct inode_block { uint32_t blocks[N_POINTERS]; };
union free_block { uint32_t next; uint8_t block[BLOCK_SIZE]; };

struct inode_layer_state {
    struct bd *lower;                   // underlying raw disk (Layer 0)
    int inode_below;                    // underlying inode
    union free_block sb;                // cached copy of block 0
};

static int alloc_block(struct inode_layer_state *s) {
    uint32_t b = s->sb.next;
    if (b == 0) return 0;                    // no free blocks
    s->lower->read(s->lower->state, s->inode_below, b, &s->sb);
    return b;
}

static void free_block(struct inode_layer_state *s, uint32_t b) {
    s->lower->write(s->lower->state, s->inode_below, b, &s->sb);
    s->sb.next = b;
    s->lower->write(s->lower->state, s->inode_below, 0, &s->sb);
}

static int simple_alloc(void *st) {
    struct inode_layer_state *s = st;
    int inode = alloc_block(s);
    if (inode == 0) return 0;
    struct inode_block ib = { 0 };
    s->lower->write(s->lower->state, s->inode_below, inode, &ib);
    return inode;
}

static int simple_size(void *st, int inode) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    for (int i = N_POINTERS - 1; i >= 0; i--)
        if (ib.blocks[i] != 0) return i + 1;
    return 0;
}

static void simple_read(void *st, int inode, int blk, void *dst) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    uint32_t b = (blk < N_POINTERS) ? ib.blocks[blk] : 0;
    if (b == 0) memset(dst, 0, BLOCK_SIZE);
    else s->lower->read(s->lower->state, s->inode_below, b, dst);
}

static void simple_write(void *st, int inode, int blk, const void *src) {
    struct inode_layer_state *s = st;
    if ((unsigned) blk >= N_POINTERS) return;
    struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    if (ib.blocks[blk] == 0) {
        int nb = alloc_block(s);
        if (nb == 0) return; // disk full
        ib.blocks[blk] = nb;
        s->lower->write(s->lower->state, s->inode_below, inode, &ib);
    }
    s->lower->write(s->lower->state, s->inode_below, ib.blocks[blk], src);
}

static void simple_free(void *st, int inode) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, s->inode_below, inode, &ib);
    for (int i = 0; i < N_POINTERS; i++)
        if (ib.blocks[i] != 0) free_block(s, ib.blocks[i]);
    free_block(s, inode);
}

void simple_init(struct bd *iface, struct inode_layer_state *s,
                struct bd *lower, int inode_below, int nblocks, int format) {
    s->lower = lower; s->inode_below = inode_below;
    if (format) {
        memset(&s->sb, 0, sizeof s->sb);
        for (int b = nblocks - 1; b > 0; b--) free_block(s, b);
    }
    else s->lower->read(s->lower->state, s->inode_below, 0, &s->sb);
    iface->state  = s;            iface->alloc  = simple_alloc;
    iface->size   = simple_size;  iface->read   = simple_read;
    iface->write  = simple_write; iface->free   = simple_free;
}
