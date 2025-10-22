#include <stdint.h>
#include <string.h>
#include "bd.h"

#define N_POINTERS (BLOCK_SIZE / 4)
struct inode_block { uint32_t blocks[N_POINTERS]; };
union free_block { uint32_t next; uint8_t block[BLOCK_SIZE]; };

struct inode_layer_state {
    struct bd *lower;                   // underlying raw disk (Layer 0)
    union free_block sb;               // cached copy of block 0
};

static int alloc_block(struct inode_layer_state *s) {
    uint32_t b = * (uint32_t *) &s->sb;
    if (b == 0) return -1;                    // no free blocks
    s->lower->read(s->lower->state, 0, b, &s->sb);
    return b;
}

static void free_block(struct inode_layer_state *s, uint32_t b) {
    s->lower->write(s->lower->state, 0, b, &s->sb);
    * (uint32_t *) s->sb_block = b;
    s->lower->write(s->lower->state, 0, 0, &s->sb);
}

static int simply_size(void *st, int inode) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, 0, inode, &ib);
    int n = 0; while (n < N_POINTERS && ib.blocks[n]) n++;
    return n;
}

static void simply_read(void *st, int inode, int blk, void *dst) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, 0, inode, &ib);
    uint32_t b = (blk < N_POINTERS) ? ib.blocks[blk] : 0;
    if (b) s->lower->read(s->lower->state, 0, b, dst);
    else  memset(dst, 0, BLOCK_SIZE);
}

static void simply_write(void *st, int inode, int blk, const void *src) {
    struct inode_layer_state *s = st;
    if ((unsigned) blk >= N_POINTERS) return;
    struct inode_block ib;
    s->lower->read(s->lower->state, 0, inode, &ib);
    if (!ib.blocks[blk]) {
        int nb = alloc_block(s);
        if (nb < 0) return; // disk full
        ib.blocks[blk] = nb;
        s->lower->write(s->lower->state, 0, inode, &ib);
    }
    s->lower->write(s->lower->state, 0, ib.blocks[blk], src);
}

static void simply_delete(void *st, int inode) {
    struct inode_layer_state *s = st;
    struct inode_block ib;
    s->lower->read(s->lower->state, 0, inode, &ib);
    for (int i = 0; i < N_POINTERS; i++)
        if (ib.blocks[i] != 0) free_block(s, ib.blocks[i]);
    free_block(s, inode);
}

void simple_init(struct bd *iface, struct inode_layer_state *st,
                    struct bd *lower, int nblocks, int format) {
    st->lower = lower;
    memset(&st->sb, 0, sizeof st->sb);
    if (format) for (int b = nblocks - 1; b > 0; b--) free_block(b);
    else s->lower->read(s->lower->state, 0, 0, &s->sb);
    iface->state  = st;
    iface->size   = simply_size;
    iface->read   = simply_read;
    iface->write  = simply_write;
    iface->delete = simply_delete;
}
