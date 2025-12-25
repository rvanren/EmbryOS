#include "embryos.h"

extern void stat_get(struct flat *fs, int file, struct stat_entry *st);
extern void stat_put(struct flat *fs, int file, struct stat_entry *st);

int flat_read(struct flat *fs, int file, int off, void *dst, int n) {
    L4(L_NORM, L_FLAT_READ, file, off, (uintptr_t) dst, n);
    struct stat_entry st; stat_get(fs, file, &st);
    if (off >= st.size) return 0;
    if (off + n > st.size) n = st.size - off;
    char *d = dst;
    int blk = off / BLOCK_SIZE, pos = off % BLOCK_SIZE, remaining = n;
    while (remaining > 0) {
        static char buf[BLOCK_SIZE];
        fs->lower->read(fs->lower->state, st.inode, blk, buf);
        int chunk = BLOCK_SIZE - pos;
        if (chunk > remaining) chunk = remaining;
        memcpy(d, buf + pos, chunk);
        d += chunk; remaining -= chunk; blk++; pos = 0;
    }
    return n;
}

int flat_write(struct flat *fs, int file, int off, const void *src, int n) {
    L4(L_NORM, L_FLAT_WRITE, file, off, (uintptr_t) src, n);
    struct stat_entry st; stat_get(fs, file, &st);
    const char *s = src;
    int blk = off / BLOCK_SIZE, pos = off % BLOCK_SIZE, written = 0;
    while (written < n) {
        static char buf[BLOCK_SIZE];
        int chunk = BLOCK_SIZE - pos;
        if (chunk > n - written) chunk = n - written;
        if (chunk < BLOCK_SIZE)
            fs->lower->read(fs->lower->state, st.inode, blk, buf);
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
    L1(L_NORM, L_FLAT_DELETE, file);
    struct stat_entry st; stat_get(fs, file, &st);
    fs->lower->free(fs->lower->state, st.inode);
    memset(&st, 0, sizeof st);
    stat_put(fs, file, &st);
}
