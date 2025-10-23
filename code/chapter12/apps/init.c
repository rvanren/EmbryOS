#include "syslib.h"
#include "string.h"
#include "screen.h"
#include "dir.h"

void printf(struct screen *screen, const char *s) {
    while (*s != 0) screen_putchar(screen, *s++);
}

void main(void) {
    struct screen screen;
    char line[128];

    screen_init(&screen);
    screen_sync(&screen);

    printf(&screen, "Initializing directory...\n");

    dir_create("init", 2);
    dir_create("shell", 3);
    dir_create("pretty", 4);
    dir_create("crash", 5);
    dir_create("ls", 6);

    printf(&screen, "System ready.\n");

    user_spawn(dir_lookup("shell"), 0, 0, 40, 12);
    user_exit();
}
