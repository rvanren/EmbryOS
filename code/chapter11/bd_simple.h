#pragma once

#include "bd.h"

#define N_POINTERS (BLOCK_SIZE / 4)

struct inode_block { uint32_t blocks[N_POINTERS]; };
union free_block   { uint32_t next; struct block block; };

struct inode_layer_state {
    struct bd *lower;       // underlying block device
    int inode_below;        // underlying inode
    union free_block sb;    // cached superblock (block 0)
};

void simple_init(struct bd *iface, struct inode_layer_state *s,
                 struct bd *lower, int inode_below,
                 int nblocks, int format);
int  simple_alloc(void *st);
int  simple_size(void *st, int inode);
void simple_read(void *st, int inode, int blk, void *dst);
void simple_write(void *st, int inode, int blk, const void *src);
void simple_free(void *st, int inode);

int simple_alloc_block(struct inode_layer_state *s);
void simple_free_block(struct inode_layer_state *s, int b);

void simple_init(struct bd *iface, struct inode_layer_state *s,
                struct bd *lower, int inode_below, int format);
