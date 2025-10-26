#include "shell_aux.c"      // rest of program

void main(void) {
    char line[128];

    // Because we are using position-independent code, we can't use pointers
    // to objects in global/static data. Hence this awkward initialization.
    rects[0] = (struct rect){ "ul",  0,  0, 39, 11 };
    rects[1] = (struct rect){ "ur", 40,  0, 39, 11 };
    rects[2] = (struct rect){ "ll",  0, 12, 39, 11 };
    rects[3] = (struct rect){ "lr", 40, 12, 39, 11 };

    for (;;) {
        printf("$ ");
        readline(line, sizeof(line));
        exec(line);
    }
}
