#include <stdint.h>
#include "bd.h"
#include "stat.h"
#include "flat.h"
#include "string.h"

#define STAT_PER_BLOCK (BLOCK_SIZE / sizeof(struct stat_entry))

static void stat_get(struct flat *fs, int file, struct stat_entry *st) {
    int blk = file / STAT_PER_BLOCK;
    struct stat_entry buf[STAT_PER_BLOCK];
    fs->lower->read(fs->lower->state, fs->stat_inode, blk, buf);
    *st = buf[file % STAT_PER_BLOCK];
}

static void stat_put(struct flat *fs, int file, struct stat_entry *st) {
    int blk = file / STAT_PER_BLOCK;
    struct stat_entry buf[STAT_PER_BLOCK];
    fs->lower->read(fs->lower->state, fs->stat_inode, blk, buf);
    buf[file % STAT_PER_BLOCK] = *st;
    fs->lower->write(fs->lower->state, fs->stat_inode, blk, buf);
}

int flat_create(struct flat *fs) {
    int inode = fs->lower->alloc(fs->lower->state);
    if (inode == 0) return 0;
    struct stat_entry st = (struct stat_entry) { .inode = inode, .size = 0 };
    int file = 0;
    while (1) {
        struct stat_entry tmp;
        stat_get(fs, file, &tmp);
        if (tmp.inode == 0) break;
        file++;
    }
    stat_put(fs, file, &st);
    return file;
}

int flat_size(struct flat *fs, int file) {
    struct stat_entry st; stat_get(fs, file, &st);
    return st.size;
}

int flat_read(struct flat *fs, int file, int off, void *dst, int n) {
    struct stat_entry st; stat_get(fs, file, &st);
    if (off >= st.size) return 0;
    if (off + n > st.size) n = st.size - off;
    char *d = dst;
    int blk = off / BLOCK_SIZE, pos = off % BLOCK_SIZE, remaining = n;
    while (remaining > 0) {
        char buf[BLOCK_SIZE];
        fs->lower->read(fs->lower->state, st.inode, blk, buf);
        int chunk = BLOCK_SIZE - pos;
        if (chunk > remaining) chunk = remaining;
        memcpy(d, buf + pos, chunk);
        d += chunk; remaining -= chunk; blk++; pos = 0;
    }
    return n;
}

int flat_write(struct flat *fs, int file, int off, const void *src, int n) {
    struct stat_entry st; stat_get(fs, file, &st);
    const char *s = src;
    int blk = off / BLOCK_SIZE, pos = off % BLOCK_SIZE, written = 0;
    while (written < n) {
        char buf[BLOCK_SIZE];
        fs->lower->read(fs->lower->state, st.inode, blk, buf);
        int chunk = BLOCK_SIZE - pos;
        if (chunk > n - written) chunk = n - written;
        memcpy(buf + pos, s + written, chunk);
        fs->lower->write(fs->lower->state, st.inode, blk, buf);
        written += chunk; blk++; pos = 0;
    }
    if (off + n > st.size) {
        st.size = off + n;
        stat_put(fs, file, &st);
    }
    return written;
}

void flat_delete(struct flat *fs, int file) {
    struct stat_entry st; stat_get(fs, file, &st);
    fs->lower->free(fs->lower->state, st.inode);
    memset(&st, 0, sizeof st);
    stat_put(fs, file, &st);
}

void flat_init(struct flat *fs, struct bd *lower, int format) {
    fs->lower = lower;
    if (format) {
        // Reserve inode #1 for the stat table (first alloc after your format loop)
        int ino = lower->alloc(lower->state);
        // Optionally assert if you like:
        // if (ino != 1) panic("unexpected stat inode");

        fs->stat_inode = ino;

        // Zero all stat blocks
        int nblocks = lower->size(lower->state, fs->stat_inode);
        struct stat_entry zero[STAT_PER_BLOCK] = {0};
        for (int b = 0; b < nblocks; b++)
            lower->write(lower->state, fs->stat_inode, b, zero);

        // File 0 describes the stat table itself
        struct stat_entry st0 = { .inode = (uint32_t)fs->stat_inode, .size = 0 };
        stat_put(fs, 0, &st0);
    }
    else {
        // By convention after a fresh format, the stat table lives at inode 1
        fs->stat_inode = 1;
        // (If you later want to make this robust, you can store stat_inode in file 0
        // and read it back; for now the convention keeps it simple.)
    }
}
