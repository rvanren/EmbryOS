#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void read(char *file) {
    int f = dir_lookup(file);
    if (f < 0) {
        printf("%s: No such file\n", file);
        return;
    }
    for (int off = 0;; off++) {
        char c;
        if (user_read(f, off, &c, 1) != 1) break;
        putchar(c);
    }
}

void main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) read(argv[i]);
}
