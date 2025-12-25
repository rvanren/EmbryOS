#include "syslib.h"
#include "string.h"
#include "dir.h"
#include "stdio.h"

#define N_RECTS 4

struct rect { const char *name; char x, y, w, h; };

struct rect rects[N_RECTS];     // list of windows
int default_rec = 1;            // upper right

void exec(char *line) {
    char *argv[64], *ptr = line;
    int argc = 0;

    for (;;) {
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == 0 || *ptr == '\n') { *ptr = 0; break; }
        argv[argc++] = ptr;
        while (*ptr != 0 && *ptr != '\n' && *ptr != ' ' && *ptr != '\t') ptr++;
        if (*ptr == 0 || *ptr == '\n') { *ptr++ = 0; break; }
        *ptr++ = 0;
    }
    if (argc == 0) return;
    if (strcmp(argv[0], "exit") == 0) user_exit();
    if (strcmp(argv[0], "quit") == 0) user_exit();

    int r = 0, i = 1;
    while (r < N_RECTS && strcmp(rects[r].name, argv[0]) != 0) r++;
    if (r == N_RECTS) { r = default_rec; i = 0; argc--; }
    else if (argc == 1) { default_rec = r; return; }
    int f = dir_lookup(argv[i]);
    if (f < 0) {
        printf("Unknown application '%s'\n", argv[i]);
        return;
    }
    user_spawn(f, rects[r].x, rects[r].y, rects[r].w, rects[r].h,
                            argv[i], ptr - argv[i]);
}

void main(void) {
    printf("Welcome to the EmbryOS shell.\n");
    printf("Type 'help' for help.\n");
    rects[0] = (struct rect){ "ul",  0,  0, 39, 11 };
    rects[1] = (struct rect){ "ur", 40,  0, 39, 11 };
    rects[2] = (struct rect){ "ll",  0, 12, 39, 11 };
    rects[3] = (struct rect){ "lr", 40, 12, 39, 11 };
    for (;;) {
        char line[128];
        printf("$ ");
        if (!readline(line, sizeof(line))) break;
        exec(line);
    }
}
