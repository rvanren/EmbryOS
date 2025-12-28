#include "embryos.h"

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
    int inode = fs->lower->alloc(fs->lower->state), file = 0;
    if (inode == 0) return 0;
    struct stat_entry st = (struct stat_entry) { .inode = inode, .size = 0 };
    while (1) {     // find an unused entry
        struct stat_entry tmp;
        stat_get(fs, file, &tmp);
        if (tmp.inode == 0) break;
        file++;
    }
    L2(L_NORM, L_FLAT_CREATE, file, inode);
    stat_put(fs, file, &st);
    return file;
}

int flat_size(struct flat *fs, int file) {
    struct stat_entry st;
    stat_get(fs, file, &st);
    L2(L_NORM, L_FLAT_SIZE, file, st.size);
    return st.size;
}

void flat_init(struct flat *fs, struct bd *lower, int format) {
    L1(L_BASE, L_FLAT_INIT, format);
    fs->lower = lower;
    if (format) {
        fs->stat_inode = lower->alloc(lower->state);
        if (fs->stat_inode != 1) die("flat_init: bad stat inode");
        struct stat_entry st0 = { .inode = fs->stat_inode, .size = 0 };
        stat_put(fs, 0, &st0);
    }
    else fs->stat_inode = 1;
}
