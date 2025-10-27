#include "syslib.h"
#include "stdio.h"
#include "dir.h"

char README[] =
//   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    "This is EmbryOS\n"
    "Type help in the shell for help\n"
    "These are the current apps:\n"
#ifdef notdef
    "* cat [files] - output files\n"
#endif
    "* life        - Game of Life\n"
    "* ls          - directory listing\n"
    "* shell       - command shell\n"
    "* snake       - snake game\n";

void main(void) {
    char line[128];

    printf("Initializing directory...\n");

    // This should correspond to the order in apps.mk
    dir_create("init",   2);    // init must be 2
    dir_create("splash", 3);
    dir_create("life",   4);
    dir_create("ls",     5);
    dir_create("shell",  6);
    dir_create("snake",  7);
    dir_create("crash",  8);
#ifdef notdef
    dir_create("cat",    9);
    dir_create("help",  10);
#endif

    int readme = user_create();
    user_write(readme, 0, README, sizeof(README) - 1);
    dir_create("README", readme);
    user_spawn(dir_lookup("shell"),   0, 0, 40, 12, 0, 0);
    user_spawn(dir_lookup("splash"), 40, 0, 40, 12, 0, 0);
}
