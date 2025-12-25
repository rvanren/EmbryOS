#pragma once

#define BLOCK_SIZE  2048

struct block { char bytes[BLOCK_SIZE]; };

// Block device interface.  Each block device has a device-specific
// state and a set of interface functions.  Abstractly, each block
// device offers 1 or more 'inodes', each of which is an array of
// blocks.
struct bd {
    void *state;     // depends on implementation of this interface
    int  (*alloc)(void *state);                 // allocate an inode
    int  (*size)(void *state, int inode);       // size in blocks
    void (*read)(void *state, int inode, int blk, void *dst);
    void (*write)(void *state, int inode, int blk, const void *src);
    void (*free)(void *state, int inode);       // free an inode
};
