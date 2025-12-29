#include "embryos.h"
#include "dir.h"

extern struct flat flat_fs;

static uword_t selfie_brk(struct trap_frame *tf, uword_t brk) {
    struct pcb *self = sched_self();
    L1(L_NORM, L_SELFIE_BREAK, brk);
    if (brk >= tf->usp - 4 * PAGE_SIZE)
        die("selfie_brk: out of virtual memory");
    if (VM_START <= brk) self->selfie.brk = brk;
    return self->selfie.brk;
}

static int selfie_dir_lookup(const char *name) {
    extern struct flat flat_fs;
    struct dirent e;
    int n = flat_size(&flat_fs, ROOT_DIR) / sizeof e;
    for (int i = 0; i < n; i++) {
        flat_read(&flat_fs, ROOT_DIR, i * sizeof e, &e, sizeof e);
        if (e.file != 0) {
            e.name[NAME_LEN - 1] = 0;
            if (strcmp(e.name, name) == 0) return e.file;
        }
    }
    return -1;
}

static uword_t selfie_open(char *name, uword_t flags, uword_t mode) {
    struct pcb *self = sched_self();
    if ((flags & 0x3) == 0) {		// open for reading
        int file = selfie_dir_lookup(name);
        if (file < 0) return -1;
        for (int fd = SELFIE_MIN_FILE; fd < SELFIE_MAX_FILE; fd++) {
            struct selfie_file *sf = &self->selfie.files[fd - SELFIE_MIN_FILE];
            if (sf->file == 0) {
                sf->file = file;
                sf->pos = 0;
                L2(L_NORM, L_SELFIE_OPEN_READ, (uintptr_t) name, fd);
                return fd;
            }
        }
        L2(L_NORM, L_SELFIE_OPEN_READ, (uintptr_t) name, -1);
    }
    if ((flags & 0x3) == 1) {		// open for writing
        extern struct flat flat_fs;
        struct dirent e;
        int n = flat_size(&flat_fs, ROOT_DIR) / sizeof e;
        int empty = -1;
        for (int i = 0; i < n; i++) {
            flat_read(&flat_fs, ROOT_DIR, i * sizeof e, &e, sizeof e);
            if (e.file != 0) {
                e.name[NAME_LEN - 1] = 0;
                if (strcmp(e.name, name) == 0) {
                    flat_delete(&flat_fs, e.file);
                    if (empty < 0) empty = i;
                    break;
                }
            }
            else if (empty < 0) empty = i;
        }
        if (empty < 0) empty = n;
        for (int fd = SELFIE_MIN_FILE; fd < SELFIE_MAX_FILE; fd++) {
            struct selfie_file *sf = &self->selfie.files[fd - SELFIE_MIN_FILE];
            if (sf->file == 0) {
                sf->file = flat_create(&flat_fs);
                sf->pos = 0;
                strncpy(e.name, name, NAME_LEN - 1);
                e.name[NAME_LEN - 1] = 0;
                e.file = sf->file;
                flat_write(&flat_fs, ROOT_DIR, empty * sizeof e, &e, sizeof(e));
                L2(L_NORM, L_SELFIE_OPEN_WRITE, (uintptr_t) name, fd);
                return fd;
            }
        }
        L2(L_NORM, L_SELFIE_OPEN_WRITE, (uintptr_t) name, -1);
    }
    return -1;
}

static uword_t selfie_read(uword_t fd, char *str, uword_t size) {
    struct pcb *self = sched_self();
    if (fd < SELFIE_MIN_FILE || fd >= SELFIE_MAX_FILE) die("selfie_read: bad fd");
    struct selfie_file *sf = &self->selfie.files[fd - SELFIE_MIN_FILE];
    if (sf->file == 0) die("file not open");
    int total = 0;

    // First empty out the buffer
    if (sf->buf_size > 0) {
        int copy = sf->buf_size < size ? sf->buf_size : size;
        memcpy(str, &sf->buf[sf->buf_pos], copy);
        sf->pos += copy; sf->buf_size -= copy; sf->buf_pos += copy;
        if (size == copy) {
            L4(L_NORM, L_SELFIE_READ, fd, (uintptr_t) str, size, copy);
            return copy;
        }
        total = copy;
    }

    // The user wants more.  If just a little bit first replenish the buffer
    if (size - total < SELFIE_BUF) {
        sf->buf_size = flat_read(&flat_fs, sf->file, sf->pos, sf->buf, SELFIE_BUF);
	int copy = sf->buf_size < size - total ? sf->buf_size : size - total;
        memcpy(str + total, sf->buf, copy);
	sf->buf_pos = copy;
	sf->buf_size -= copy;
        sf->pos += copy;
        L4(L_NORM, L_SELFIE_READ, fd, (uintptr_t) str, size, total + copy);
        return total + copy;
    }

    // Otherwise don't use the buffer
    int n = flat_read(&flat_fs, sf->file, sf->pos, str, size);
    sf->pos += n;
    L4(L_NORM, L_SELFIE_READ, fd, (uintptr_t) str, size, n);
    return total + n;
}

static void selfie_put(int col, int row, char c) {
    struct pcb *self = sched_self();
    self->selfie.window[row][col] = c;
    proc_put(self, col, row, CELL(c, ANSI_BLACK, ANSI_GREEN));
}

static void selfie_scroll() {
    struct pcb *self = sched_self();
    for (int row = 0; row < SELFIE_NROWS - 1; row++)
        for (int col = 0; col < SELFIE_NCOLS; col++)
            selfie_put(col, row, self->selfie.window[row + 1][col]);
    for (int col = 0; col < SELFIE_NCOLS; col++)
        selfie_put(col, SELFIE_NROWS - 1, ' ');
    self->selfie.col = 0;
}

static uword_t selfie_write(uword_t fd, char *str, uword_t size) {
    struct pcb *self = sched_self();
    L3(L_NORM, L_SELFIE_WRITE, fd, (uintptr_t) str, size);
    if (fd == 1 || fd == 2) {
        for (int i = 0; i < size; i++) {
            if (str[i] == 0) continue;
            if (str[i] == '\n') selfie_scroll();
            else {
                selfie_put(self->selfie.col, SELFIE_NROWS - 1, str[i]);
                self->selfie.col++;
                if (self->selfie.col == SELFIE_NCOLS) selfie_scroll();
            }
        }
        return size;
    }
    else {
        if (fd < SELFIE_MIN_FILE || fd >= SELFIE_MAX_FILE) die("selfie_write: bad fd");
        struct selfie_file *sf = &self->selfie.files[fd - SELFIE_MIN_FILE];
        if (sf->file == 0) die("file not open");
        int n = flat_write(&flat_fs, sf->file, sf->pos, str, size);
        sf->pos += n;
        return n;
    }
}

void selfie_initialize() {
    struct pcb *self = sched_self();
    for (int row = 0; row < SELFIE_NROWS; row++)
        for (int col = 0; col < SELFIE_NCOLS; col++) selfie_put(col, row, ' ');
    self->selfie.brk = (VM_START +
           flat_size(&flat_fs, self->executable) - PAGE_SIZE + 7) & ~7;
    self->selfie.initialized = 1;
    L1(L_NORM, L_SELFIE_INIT, self->selfie.brk);
}

void selfie_syscall_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    if (!self->selfie.initialized) selfie_initialize();
    switch (tf->a7) {
    case 56:
        tf->a0 = selfie_open((char *) tf->a1, tf->a2, tf->a3);
        break;
    case 63:
        tf->a0 = selfie_read(tf->a0, (char *) tf->a1, tf->a2);
        break;
    case 64:
        tf->a0 = selfie_write(tf->a0, (char *) tf->a1, tf->a2);
        break;
    case 93:
        L1(L_NORM, L_SELFIE_EXIT, tf->a0);
        die("selfie ended");
        break;
    case 214:
        tf->a0 = selfie_brk(tf, tf->a0);
        break;
    default:
        die("unknown selfie call");
    }
}
