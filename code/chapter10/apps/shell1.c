#include "syslib.h"
#include "screen.h"
#include "kb.h"
#include "string.h"

#define N_RECTS 4
#define N_APPS  3

void printf(struct screen *screen, const char *s) {
    while (*s) screen_putchar(screen, *s++);
}

extern char *apps[N_APPS];
struct rect { const char *name; char x, y, w, h; };
extern struct rect rects[N_RECTS];

void exec(struct screen *screen, char *line) {
    char *argv[64];
    int argc = 0;

    for (;;) {
        while (*line == ' ' || *line == '\t') line++;
        if (*line == 0) break;
        argv[argc++] = line;
        while (*line && *line != ' ' && *line != '\t') line++;
        if (*line == 0) break;
        *line++ = 0;
    }
    if (argc == 0) return;

    if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "exit")) user_exit();
    if (!strcmp(argv[0], "help")) {
        printf(screen, "[ul|ur|ll|lr] [shell|pretty|crash]\n");
        return;
    }

    int r = 0;
    while (r < N_RECTS && strcmp(rects[r].name, argv[0])) r++;
    if (r == N_RECTS) { printf(screen, "Unknown window\n"); return; }

    if (argc == 1) { printf(screen, "Too few arguments\n"); return; }

    int a = 0;
    while (a < N_APPS && strcmp(apps[a], argv[1])) a++;
    if (a == N_APPS) { printf(screen, "Unknown app\n"); return; }

    user_spawn(a, rects[r].x, rects[r].y, rects[r].w, rects[r].h);
}
