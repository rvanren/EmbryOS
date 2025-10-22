#include "syslib.h"
#include "screen.h"
#include "string.h"

#define N_RECTS 4
#define N_APPS  3

struct rect { const char *name; char x, y, w, h; };
struct app { const char *name; int file; }

struct app apps[N_APPS];        // list of applications
struct rect rects[N_RECTS];     // list of windows

void printf(struct screen *screen, const char *s) {
    while (*s) screen_putchar(screen, *s++);
}

// Execute the given command
void exec(struct screen *screen, char *line) {
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
    if (strcmp(argv[0], "help") == 0) {
        printf(screen, "[ul|ur|ll|lr] [shell|pretty|crash]\n");
        return;
    }

    int r = 0;
    while (r < N_RECTS && strcmp(rects[r].name, argv[0]) != 0) r++;
    if (r == N_RECTS) { printf(screen, "Unknown window\n"); return; }

    if (argc == 1) { printf(screen, "Too few arguments\n"); return; }
    int a = 0;
    while (a < N_APPS && strcmp(apps[a].name, argv[1]) != 0) a++;
    if (a == N_APPS) { printf(screen, "Unknown app\n"); return; }

    user_spawn(apps[a].file, rects[r].x, rects[r].y, rects[r].w, rects[r].h);
}
