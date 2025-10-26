#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void main(void) {
    char line[128];

    printf("Initializing directory...\n");

    dir_create("init", 2);
    dir_create("shell", 3);
    dir_create("life", 4);
    dir_create("crash", 5);
    dir_create("ls", 6);
    dir_create("cat", 7);
    dir_create("snake", 8);
    dir_create("splash", 9);

    int readme = user_create();
    char contents[] = "This is EmbryOS\n";
    user_write(readme, 0, contents, sizeof(contents) - 1);
    dir_create("README", readme);

    printf("System ready.\n");

    user_spawn(dir_lookup("shell"), 0, 0, 40, 12, 0, 0);
}
