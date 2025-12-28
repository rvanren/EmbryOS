#pragma once

#define BLOCK_SIZE  2048

struct block { char bytes[BLOCK_SIZE]; };

// Block device interface.  Each block device has a device-specific
// state and a set of interface functions.  Abstractly, each block
// device offers 1 or more 'inodes', each of which is a fixed-size
// array of null-initialized blocks.
struct bd {
    void *state;     // depends on implementation of this interface
    int  (*alloc)(void *state);                 // allocate an inode
    int  (*size)(void *state, int inode);       // maximum size in blocks
    void (*read)(void *state, int inode, int blk, void *dst);
    void (*write)(void *state, int inode, int blk, const void *src);
    void (*free)(void *state, int inode);       // free an inode
};

extern const struct block bd_null_block;

struct block *bd_alloc(void);
void bd_free(struct block *b);
void bd_init(void);
