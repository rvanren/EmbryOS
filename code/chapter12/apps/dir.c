#include "syslib.h"
#include "string.h"
#include "dir.h"

int dir_lookup(const char *name) {
    struct dirent e;
    int n = user_size(dir_file) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(dir_file, i * sizeof e, &e, sizeof e);
        if (strcmp(e.name, name) == 0)
            return e.file;
    }
    return -1;
}

int dir_create(const char *name, int file) {
    if (dir_lookup(name) >= 0) return -1;     // already exists
    struct dirent e;
    memset(&e, 0, sizeof e);
    strncpy(e.name, name, NAME_LEN - 1);
    e.file = file;
    user_write(dir_file, user_size(dir_file), &e, sizeof e);
    return file;
}

void dir_delete(const char *name) {
    struct dirent e;
    int n = user_size(dir_file) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(dir_file, i * sizeof e, &e, sizeof e);
        if (strcmp(e.name, name) == 0) {
            user_delete(e.file);
            e.file = -1;
            e.name[0] = 0;
            user_write(dir_file, i * sizeof e, &e, sizeof e);
            return;
        }
    }
}

void dir_list(void (*fn)(const char *name, int file)) {
    struct dirent e;
    int n = user_size(dir_file) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(dir_file, i * sizeof e, &e, sizeof e);
        if (e.file > 0 && e.name[0])
            fn(e.name, e.file);
    }
}
