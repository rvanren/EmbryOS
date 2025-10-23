#include "syslib.h"
#include "string.h"
#include "dir.h"

#define N_RECTS 4

struct rect { const char *name; char x, y, w, h; };
struct app { const char *name; int file; };

struct rect rects[N_RECTS];     // list of windows

// Execute the given command
void exec(char *line) {
    char *argv[64];
    int argc = 0;

    for (;;) {
        while (*line == ' ' || *line == '\t') line++;
        if (*line == 0) break;
        argv[argc++] = line;
        while (*line != 0 && *line != ' ' && *line != '\t') line++;
        if (*line == 0) break;
        *line++ = 0;
    }
    if (argc == 0) return;

    if (strcmp(argv[0], "exit") == 0) user_exit();
    if (strcmp(argv[0], "quit") == 0) user_exit();

    int r = 0;
    while (r < N_RECTS && strcmp(rects[r].name, argv[0]) != 0) r++;
    if (r == N_RECTS) {
        printf("Usage: [ul|ur|ll|lr] command\n");
        return;
    }

    if (argc == 1) { printf("Too few arguments\n"); return; }
    int f = dir_lookup(argv[1]);
    if (f < 0) {
        printf("Unknown application\n");
        return;
    }

    user_spawn(f, rects[r].x, rects[r].y, rects[r].w, rects[r].h);
}
