#include "syslib.h"
#include "string.h"
#include "dir.h"

int dir_lookup(const char *name) {
    struct dirent e;
    int n = user_size(ROOT_DIR) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(ROOT_DIR, i * sizeof e, &e, sizeof e);
        if (e.file != 0) {
            e.name[NAME_LEN - 1] = 0;
            if (strcmp(e.name, name) == 0) return e.file;
        }
    }
    return -1;
}

int dir_create(const char *name, int file) {
    if (strlen(name) >= NAME_LEN - 1) return -1;
    struct dirent e;
    int n = user_size(ROOT_DIR) / sizeof e, unused = n;
    for (int i = 0; i < n; i++) {
        user_read(ROOT_DIR, i * sizeof e, &e, sizeof e);
        if (e.file == 0) unused = i;
        else {
            e.name[NAME_LEN - 1] = 0;
            if (strcmp(e.name, name) == 0) return -1;
        }
    }
    e.file = file;
    strcpy(e.name, name);
    user_write(ROOT_DIR, unused * sizeof(e), &e, sizeof(e));
    return file;
}

void dir_delete(const char *name) {
    struct dirent e;
    int n = user_size(ROOT_DIR) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(ROOT_DIR, i * sizeof e, &e, sizeof e);
        e.name[NAME_LEN - 1] = 0;
        if (e.file != 0) {
            e.name[NAME_LEN - 1] = 0;
            if (strcmp(e.name, name) == 0) {
                user_delete(e.file);
                memset(&e, 0, sizeof(e));
                user_write(ROOT_DIR, i * sizeof e, &e, sizeof e);
                break;
            }
        }
    }
}

void dir_list(void (*fn)(const char *name, int file)) {
    struct dirent e;
    int n = user_size(ROOT_DIR) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(ROOT_DIR, i * sizeof e, &e, sizeof e);
        if (e.file != 0) {
            e.name[NAME_LEN - 1] = 0;
            fn(e.name, e.file);
        }
    }
}
