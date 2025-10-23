#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void main(void) {
    char line[128];

    printf("Initializing directory...\n");

    dir_create("init", 2);
    dir_create("shell", 3);
    dir_create("pretty", 4);
    dir_create("crash", 5);
    dir_create("ls", 6);

    printf("System ready.\n");

    user_spawn(dir_lookup("shell"), 0, 0, 40, 12, 0, 0);
    user_exit();
}
