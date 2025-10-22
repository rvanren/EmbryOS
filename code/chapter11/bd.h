#pragma once

#define BLOCK_SIZE  512

struct block { char bytes[BLOCK_SIZE]; };

struct bd {
    void *state;     // depends on implementation of this interface
    int  (*alloc)(void*);                       // allocate an inode
    int  (*size)(void *state, int inode);       // size in blocks
    void (*read)(void *state, int inode, int blk, void *dst);
    void (*write)(void *state, int inode, int blk, const void *src);
    void (*free)(void*, int inode);             // free an inode
};
