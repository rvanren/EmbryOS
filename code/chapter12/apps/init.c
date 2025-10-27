#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void init_main(void) {
    printf("Initializing directory...\n");

    // This should correspond to the order in apps.mk
    dir_create("init",   2);    // init must be 2
    dir_create("splash", 3);
    dir_create("life",   4);
    dir_create("snake",  5);
    dir_create("shell",  6);
    dir_create("crash",  7);
    dir_create("ls",     8);
    user_spawn(dir_lookup("shell"),   0,  0, 40, 12, 0, 0);
    user_spawn(dir_lookup("splash"), 40,  0, 40, 12, 0, 0);
}

void main() { init_main(); }
