struct disk_state {
    int nblocks;
    uint8_t *data;
};

static int  disk_size(void *st, int inode) {
    struct disk_state *d = st;
    return d->nblocks;
}

static void disk_read(void *st, int inode, int blk, void *dst) {
    struct disk_state *d = st;
    memcpy(dst, d->data + blk * BLOCK_SIZE, BLOCK_SIZE);
}

static void disk_write(void *st, int inode, int blk, const void *src) {
    struct disk_state *d = st;
    memcpy(d->data + blk * BLOCK_SIZE, src, BLOCK_SIZE);
}

static void disk_delete(void *st, int inode) { }

void bd_disk_init(struct bd *iface, struct disk_state *state,
                  void *mem, int nblocks) {
    state->data = mem;
    state->nblocks = nblocks;
    iface->state  = state;
    iface->size   = disk_size;
    iface->read   = disk_read;
    iface->write  = disk_write;
    iface->delete = disk_delete;
}
