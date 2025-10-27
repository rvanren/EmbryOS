#include <stdint.h>
#include "bd.h"
#include "stat.h"
#include "flat.h"

void stat_get(struct flat *fs, int file, struct stat_entry *st) {
    int blk = file / STAT_PER_BLOCK;
    static struct stat_entry buf[STAT_PER_BLOCK];
    fs->lower->read(fs->lower->state, fs->stat_inode, blk, buf);
    *st = buf[file % STAT_PER_BLOCK];
}

void stat_put(struct flat *fs, int file, struct stat_entry *st) {
    int blk = file / STAT_PER_BLOCK;
    static struct stat_entry buf[STAT_PER_BLOCK];
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

void flat_init(struct flat *fs, struct bd *lower, int format) {
    fs->lower = lower;
    if (format) {
        fs->stat_inode = lower->alloc(lower->state);
        int nblocks = lower->size(lower->state, fs->stat_inode);
        struct stat_entry zero[STAT_PER_BLOCK] = {0};
        for (int b = 0; b < nblocks; b++)
            lower->write(lower->state, fs->stat_inode, b, zero);
        struct stat_entry st0 = { .inode = fs->stat_inode, .size = 0 };
        stat_put(fs, 0, &st0);
    }
    else fs->stat_inode = 1;
    // if (fs->stat_inode != 1) panic("unexpected stat inode");
}
