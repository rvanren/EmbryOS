#include "embryos.h"

int simple_alloc_block(struct simple_state *s) {
    int b = s->sb.next;
    L1(L_FREQ, L_SIMPLE_ALLOC_BLOCK, b);
    if (b == 0) die("disk full");
    s->lower->read(s->lower->state, s->inode_below, b, &s->sb);
    return b;
}

void simple_free_block(struct simple_state *s, int b) {
    L1(L_FREQ, L_SIMPLE_FREE_BLOCK, b);
    s->lower->write(s->lower->state, s->inode_below, b, &s->sb);
    s->sb.next = b;
    s->lower->write(s->lower->state, s->inode_below, 0, &s->sb);
}

void simple_init(struct bd *iface, struct simple_state *s,
                struct bd *lower, int inode_below, int format) {
    s->lower = lower; s->inode_below = inode_below;
    int nblocks = lower->size(s->lower->state, inode_below);
    if (format) {
        L2(L_BASE, L_SIMPLE_FORMAT_START, inode_below, nblocks);
        memset(&s->sb, 0, sizeof s->sb);
        for (int b = nblocks - 1; b > 0; b--) simple_free_block(s, b);
        L0(L_BASE, L_SIMPLE_FORMAT_END);
    }
    else s->lower->read(s->lower->state, s->inode_below, 0, &s->sb);
    iface->state  = s;            iface->alloc  = simple_alloc;
    iface->size   = simple_size;  iface->read   = simple_read;
    iface->write  = simple_write; iface->free   = simple_free;
}
