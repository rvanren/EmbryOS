#include "syslib.h"
#include "screen.h"
#include "kb.h"
#include "string.h"

#define N_RECTS 4
#define N_APPS  3

extern void exec(struct screen *, char *);

char *apps[N_APPS];
struct rect { const char *name; char x, y, w, h; };
struct rect rects[N_RECTS];

void main(void) {
    struct screen screen;
    char line[128];

    // initialize window layout and apps (no static pointers)
    rects[0] = (struct rect){ "ul",  0,  0, 40, 12 };
    rects[1] = (struct rect){ "ur", 40,  0, 40, 12 };
    rects[2] = (struct rect){ "ll",  0, 12, 40, 12 };
    rects[3] = (struct rect){ "lr", 40, 12, 40, 12 };
    apps[0] = "shell"; apps[1] = "pretty"; apps[2] = "crash";

    screen_init(&screen);
    screen_sync(&screen);

    for (;;) {
        printf(&screen, "$ ");
        kb_readline(&screen, line, sizeof(line));
        exec(&screen, line);
    }
}
