#pragma once

#include "bd.h"

// A "simple block device" is a block device where each inode
// can have a variable number of blocks (up to N_POINTERS as
// defined here). There are four types of blocks: the superblock
// (block 0), inode blocks, data blocks, and free blocks.  There
// is one inode block per inode, and it contains up to N_POINTERS
// references to data blocks (holes supported).  The superblock
// has the same type as a free block and points to a straightforward
// free list of free blocks.

#define SIMPLE_N_POINTERS (BLOCK_SIZE / 4)

struct simple_inode_block { uint32_t blocks[SIMPLE_N_POINTERS]; };
union simple_free_block   { uint32_t next; struct block block; };

struct simple_state {
    struct bd *lower;            // underlying block device
    int inode_below;             // underlying inode
    union simple_free_block sb;  // cached superblock (block 0)
};

// The methods implementing the "block device" (bd) interface
int  simple_alloc(void *st);
int  simple_size(void *st, int inode);
void simple_read(void *st, int inode, int blk, void *dst);
void simple_write(void *st, int inode, int blk, const void *src);
void simple_free(void *st, int inode);

// Handy internal interfaces
int simple_alloc_block(struct simple_state *s);
void simple_free_block(struct simple_state *s, int b);

// Called to initialize a "simple" block device.  iface points to
// the interface, s points to where the state is kept, lower points
// to the layer below, inode_below is the inode_below which the
// simple layer stores its data, and format is a boolean indicating
// if the simple layer should initialize the data structure from
// scratch or expect an existing one.
void simple_init(struct bd *iface, struct simple_state *s,
                struct bd *lower, int inode_below, int format);
