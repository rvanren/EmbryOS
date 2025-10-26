#include "syslib.h"
#include "string.h"
#include "dir.h"
#include "stdio.h"

#define N_RECTS 4

struct rect { const char *name; char x, y, w, h; };
struct app { const char *name; int file; };

struct rect rects[N_RECTS];     // list of windows
int default_rec = 1;            // upper right

// Execute the given command
void exec(char *line) {
    char *argv[64], *ptr = line;
    int argc = 0;

    for (;;) {
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == 0) break;
        argv[argc++] = ptr;
        while (*ptr != 0 && *ptr != ' ' && *ptr != '\t') ptr++;
        if (*ptr == 0) break;
        *ptr++ = 0;
    }
    if (argc == 0) return;

    if (strcmp(argv[0], "exit") == 0) user_exit();
    if (strcmp(argv[0], "quit") == 0) user_exit();

    int r = 0, i = 0;
    while (r < N_RECTS && strcmp(rects[r].name, argv[0]) != 0) r++;
    if (r == N_RECTS) { r = default_rec; i++; argc--; }
    else if (argc == 1) { default_rec = r; return; }

    int f = dir_lookup(argv[i]);
    if (f < 0) {
        printf("Unknown application\n");
        return;
    }

    user_spawn(f, rects[r].x, rects[r].y, rects[r].w, rects[r].h,
                            argv[i], ptr - argv[i]);
}
