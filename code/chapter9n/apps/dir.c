#include "syslib.h"
#include "string.h"
#include "dir.h"

#ifdef CH12

#define DIR_FILE 1

int dir_lookup(const char *name) {
    struct dirent e;
    int n = user_size(DIR_FILE) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(DIR_FILE, i * sizeof e, &e, sizeof e);
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
    user_write(DIR_FILE, user_size(DIR_FILE), &e, sizeof e);
    return file;
}

void dir_delete(const char *name) {
    struct dirent e;
    int n = user_size(DIR_FILE) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(DIR_FILE, i * sizeof e, &e, sizeof e);
        if (strcmp(e.name, name) == 0) {
            user_delete(e.file);
            e.file = -1;
            e.name[0] = 0;
            user_write(DIR_FILE, i * sizeof e, &e, sizeof e);
            return;
        }
    }
}

void dir_list(void (*fn)(const char *name, int file)) {
    struct dirent e;
    int n = user_size(DIR_FILE) / sizeof e;
    for (int i = 0; i < n; i++) {
        user_read(DIR_FILE, i * sizeof e, &e, sizeof e);
        if (e.file > 0 && e.name[0])
            fn(e.name, e.file);
    }
}

#else

int dir_lookup(const char *name) {
    if (strcmp(name, "splash") == 0) return 3;
    if (strcmp(name, "life")   == 0) return 4;
    if (strcmp(name, "shell")  == 0) return 5;
    if (strcmp(name, "snake")  == 0) return 6;
#ifdef CH10
    if (strcmp(name, "crash")  == 0) return 7;
#endif
    return -1;
}

#endif
