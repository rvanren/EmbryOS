#include "syslib.h"
#include "stdio.h"
#include "dir.h"

char README[] =
    "This is EmbryOS\n"
    "Switch focus on windows using TAB\n"
    "Type help in the shell for help";

void main(void) {
    char line[128];

    printf("Initializing directory...\n");


# The list of applications to be compiled into the kernel image is here
# Note that the kernel must know the order of these applications.
APPS = init splash cat crash help life ls shell snake

    dir_create("init",   2);    // init must be 2
    dir_create("splash", 3);
    dir_create("cat",    4);
    dir_create("crash",  5);
    dir_create("help",   6);
    dir_create("life",   7);
    dir_create("ls",     8);
    dir_create("shell",  9);
    dir_create("snake", 10);

    int readme = user_create();
    user_write(readme, 0, README, sizeof(README) - 1);
    dir_create("README", readme);
    user_spawn(dir_lookup("shell"),   0, 0, 40, 12, 0, 0);
    user_spawn(dir_lookup("splash"), 40, 0, 40, 12, 0, 0);
}
