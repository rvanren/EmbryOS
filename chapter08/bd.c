#include "embryos.h"

// Threads have only limited stack space, but the block device layers
// often need a temporary block variable.  Instead of allocating it
// on the stack, it's better to use this heap.

#define MAX_HEAP    16

union bd_free_block { union bd_free_block *next; struct block block; };
static union bd_free_block *bd_free_list;
static struct block bd_heap[MAX_HEAP];
const struct block bd_null_block;

struct block *bd_alloc(void) {
    union bd_free_block *bf = bd_free_list;
    if (bf == 0) die("bd_alloc: out of blocks");
    bd_free_list = bf->next;
    return &bf->block;
}

void bd_free(struct block *b) {
    union bd_free_block *bf = (union bd_free_block *) b;
    bf->next = bd_free_list;
    bd_free_list = bf;
}

void bd_init(void) {
    for (int i = 0; i < MAX_HEAP; i++) bd_free(&bd_heap[i]);
}
