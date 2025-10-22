#include "syslib.h"
#include "screen.h"
#include "kb.h"
#include "string.h"

#define N_RECTS     4       // number of windows on the screen
#define N_APPS      3       // number of available applications to run

void printf(struct screen *screen, const char *s) {
    while (*s != 0) screen_putchar(screen, *s++);
}

char *apps[N_APPS];
struct rect {
    const char *name;   // name of window
    char x, y, w, h;    // bounding box
};
struct rect rects[N_RECTS];

void exec(struct screen *screen, char *line) {
    char *argv[64];
    int argc = 0;

    for (;;) {
        while (*line == ' ' || *line == '\t') line++;
        if (*line == 0) break;
        argv[argc++] = line;
        while (*line != ' ' && *line != '\t' && *line != 0) line++;
        if (*line == 0) break;
        *line++ = 0;
    }
    if (argc == 0) return;

    if (strcmp(argv[0], "quit") == 0) user_exit();
    if (strcmp(argv[0], "exit") == 0) user_exit();
    if (strcmp(argv[0], "help") == 0) {
        printf(screen, "[ul|ur|ll|lr] [shell|pretty|crash]\n");
        return;
    }

    int r = 0;
    while (r < N_RECTS) {
        if (strcmp(rects[r].name, argv[0]) == 0) break;
        r++;
    }
    if (r == N_RECTS) { printf(screen, "Unknown command\n"); return; }

    if (argc == 1) { printf(screen, "Too few arguments\n"); return; }
    int app = 0;
    while (app < N_APPS) {
        if (strcmp(apps[app], argv[1]) == 0) break;
        app++;
    }
    if (app == N_APPS) { printf(screen, "Unknown app\n"); return; }
    user_spawn(app, rects[r].x,  rects[r].y, rects[r].w, rects[r].h);
}

void main(void) {
    struct screen screen;
    char line[128];

    // Because we are using position-independent code, we can't use pointers
    // to objects in global/static data. Hence this awkward initialization.
    rects[0] = (struct rect){ "ul",  0,  0, 40, 12 };
    rects[1] = (struct rect){ "ur", 40,  0, 40, 12 };
    rects[2] = (struct rect){ "ll",  0, 12, 40, 12 };
    rects[3] = (struct rect){ "lr", 40, 12, 40, 12 };
    apps[0] = "shell";
    apps[1] = "pretty";
    apps[2] = "crash";

    screen_init(&screen);
    screen_sync(&screen);
    printf(&screen, "$ ");
    for (;;) {
        kb_readline(&screen, line, sizeof(line));
        exec(&screen, line);
    }
}
