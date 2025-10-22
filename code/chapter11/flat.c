#include <stdint.h>
#include "bd.h"
#include "stat.h"
#include "flat.h"
#include "string.h"

#define STAT_PER_BLOCK (BLOCK_SIZE / sizeof(struct stat_entry))

struct flat { struct bd *lower; int stat_inode; };

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
