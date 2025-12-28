#include "embryos.h"

static int ramdisk_size(void *st, int inode) {
    struct ramdisk_state *d = st;
    return d->nblocks;
}

static int ramdisk_alloc(void *st) { return 0; }
static void ramdisk_free(void *st, int inode) { }

static void ramdisk_read(void *st, int inode, int blk, void *dst) {
    L3(L_NORM, L_RAMDISK_READ, inode, blk, (uintptr_t) dst);
    struct ramdisk_state *d = st;
    if ((unsigned) blk >= d->nblocks) die("ramdisk_read: bad offset");
    memcpy(dst, d->data + blk * BLOCK_SIZE, BLOCK_SIZE);
}

static void ramdisk_write(void *st, int inode, int blk, const void *src) {
    L3(L_FREQ, L_RAMDISK_WRITE, inode, blk, (uintptr_t) src);
    struct ramdisk_state *d = st;
    if ((unsigned) blk >= d->nblocks) die("ramdisk_write: bad offset");
    memcpy(d->data + blk * BLOCK_SIZE, src, BLOCK_SIZE);
}

void ramdisk_init(struct bd *iface, struct ramdisk_state *state,
                  void *mem, int nblocks) {
    state->data = mem;
    state->nblocks = nblocks;
    iface->state  = state;
    iface->alloc  = ramdisk_alloc;
    iface->size   = ramdisk_size;
    iface->read   = ramdisk_read;
    iface->write  = ramdisk_write;
    iface->free   = ramdisk_free;
}
