#include "syslib.h"
#include "screen.h"

void printf(struct screen *screen, const char *s) {
    while (*s != 0) putchar(screen, *s++);
}

int strcmp(const char *p, const char *q) {
    while (*p != 0 && *q != 0 && *p == *q) { p++; q++; }
    if (*p == *q) { return 0; }
    if (*p == 0) { return -1; }
    if (*q == 0) { return 1; }
    return *p - *q;
}

char *apps[4];
struct rect {
    const char *name;
    char x, y;   // top-left corner on global screen
    char w, h;   // width and height
};
struct rect rects[5];

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
    while (rects[r].name != 0) {
        if (strcmp(rects[r].name, argv[0]) == 0) break;
        r++;
    }
    if (rects[r].name == 0) {
        printf(screen, "Unknown command\n");
        return;
    }

    if (argc == 1) {
        printf(screen, "Too few arguments\n");
        return;
    }

    int app = 0;
    while (apps[app] != 0) {
        if (strcmp(apps[app], argv[1]) == 0) break;
        app++;
    }
    if (apps[app] == 0) {
        printf(screen, "Unknown app\n");
        return;
    }
    user_spawn(app, rects[r].x,  rects[r].y, rects[r].w, rects[r].h);
}

void main(void) {
    struct screen screen;
    char line[128];
    unsigned int n = 0;

    // Because of PIC, I can't have pointers-to-objects in global/static data
    // Hence this awkward initialization
    rects[0] = (struct rect){ "ul",  0,  0, 40, 12 };
    rects[1] = (struct rect){ "ur", 40,  0, 40, 12 };
    rects[2] = (struct rect){ "ll",  0, 12, 40, 12 };
    rects[3] = (struct rect){ "lr", 40, 12, 40, 12 };
    rects[4] = (struct rect){ 0,     0,  0,  0,  0 };
    apps[0] = "shell";
    apps[1] = "pretty";
    apps[2] = "crash";
    apps[3] = 0;

    screen_init(&screen);
    screen_sync(&screen);
    printf(&screen, "$ ");
    for (;;) {
        char c = user_get();
        if (c == '\r') {
            putchar(&screen, '\n');
            line[n] = 0;
            exec(&screen, line);
            n = 0;
            printf(&screen, "$ ");
        }
        else {
            putchar(&screen, c);
            if (c == '\b' || c == '\177') { if (n > 0) n--; }
	        else if (n < sizeof(line) - 1) line[n++] = c;
        }
    }
}
