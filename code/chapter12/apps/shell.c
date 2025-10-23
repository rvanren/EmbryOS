#include "shell_aux.c"      // rest of program
#include "kb.h"

void main(void) {
    struct screen screen;
    char line[128];

    // Because we are using position-independent code, we can't use pointers
    // to objects in global/static data. Hence this awkward initialization.
    rects[0] = (struct rect){ "ul",  0,  0, 40, 12 };
    rects[1] = (struct rect){ "ur", 40,  0, 40, 12 };
    rects[2] = (struct rect){ "ll",  0, 12, 40, 12 };
    rects[3] = (struct rect){ "lr", 40, 12, 40, 12 };
    apps[0] = (struct app){ .name = "shell",  .file = 1 };
    apps[1] = (struct app){ .name = "pretty", .file = 2 };
    apps[2] = (struct app){ .name = "crash",  .file = 3 };
    apps[2] = (struct app){ .name = "ls",     .file = 4 };

    // initialize windows
    screen_init(&screen);
    screen_sync(&screen);

    for (;;) {
        printf(&screen, "$ ");
        kb_readline(&screen, line, sizeof(line));
        exec(&screen, line);
    }
}
