#include <stdarg.h>
#include "syslib.h"

#define NROWS 12
#define NCOLS 40

struct cell { char c, fg, bg; };
struct screen {
    struct cell cells[NROWS][NCOLS];
    int cur_col, cur_fg, cur_bg;
};

void screen_put(int row, int col, struct cell cell) {
    user_put(row, col, cell.c, cell.fg, cell.bg);
}

void screen_init(struct screen *screen) {
    screen->cur_col = 0;
    screen->cur_fg = 0;
    screen->cur_bg = 7;
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen->cells[row][col] = (struct cell) { ' ', 0, 7 };
        }
    }
}

void screen_sync(struct screen *screen) {
    for (int row = 0; row < NROWS; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen_put(row, col, screen->cells[row][col]);
        }
    }
}

void screen_scroll(struct screen *screen) {
    for (int row = 0; row < NROWS - 1; row++) {
        for (int col = 0; col < NCOLS; col++) {
            screen->cells[row][col] = screen->cells[row + 1][col];
        }
    }
    for (int col = 0; col < NCOLS; col++) {
        screen->cells[NROWS - 1][col] = (struct cell) { ' ', 0, 7 };
    }
    screen_sync(screen);
}

void putchar(struct screen *screen, char c) {
    if (c == '\b' || c == '\177') {
        if (screen->cur_col > 0) {
            screen->cur_col--;
            user_put(NROWS - 1, screen->cur_col, ' ', screen->cur_fg, screen->cur_bg);
            screen->cells[NROWS - 1][screen->cur_col] = (struct cell) { ' ', screen->cur_fg, screen->cur_bg };
            if (screen->cur_col > 0)
                screen_put(NROWS - 1, screen->cur_col - 1, screen->cells[NROWS - 1][screen->cur_col - 1]);
        }
        return;
    }
    if (c == '\n' || screen->cur_col == NCOLS) {
        screen_scroll(screen);
        screen->cur_col = 0;
    }
    if (c != '\n') {
        user_put(NROWS - 1, screen->cur_col, c, screen->cur_fg, screen->cur_bg);
        screen->cells[NROWS - 1][screen->cur_col] = (struct cell) { c, screen->cur_fg, screen->cur_bg };
        screen->cur_col++;
    }
}

static void print_unsigned(struct screen *screen, unsigned int x, unsigned int base) {
    char buf[16];
    int i = 0;
    do {
        int digit = x % base;
        buf[i++] = (digit < 10) ? '0' + digit : 'a' + (digit - 10);
        x /= base;
    } while (x != 0);
    while (--i >= 0) putchar(screen, buf[i]);
}

void printf(struct screen *screen, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { putchar(screen, *fmt); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': { int x = va_arg(ap, int);
            if (x < 0) { putchar(screen, '-'); x = -x; }
            print_unsigned(screen, (unsigned int) x, 10);
            break;
        }
        case 'u': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(screen, x, 10);
            break;
        }
        case 'x': { unsigned int x = va_arg(ap, unsigned int);
            print_unsigned(screen, x, 16);
            break;
        }
        case 's': { char *s = va_arg(ap, char *);
            while (*s) putchar(screen, *s++);
            break;
        }
        case 'c': { int c = va_arg(ap, int);
            putchar(screen, c);
            break;
        }
        case '%':
            putchar(screen, '%');
            break;
        default:
            putchar(screen, '%'); putchar(screen, *fmt);
        }
    }
    va_end(ap);
}

int strcmp(char *p, char *q) {
    while (*p != 0 && *q != 0 && *p == *q) { p++; q++; }
    if (*p == *q) { return 0; }
    if (*p == 0) { return -1; }
    if (*q == 0) { return 1; }
    return *p - *q;
}

char *apps[] = { "shell", "pretty", "crash", 0 };
struct rect {
    char *name;
    char x, y;   // top-left corner on global screen
    char w, h;   // width and height
};
struct rect rects[] = {
    { "ul",  0,  0, 40, 12 },
    { "ur", 40,  0, 40, 12 },
    { "ll",  0, 12, 40, 12 },
    { "lr", 40, 12, 40, 12 },
    { 0,     0,  0,  0,  0 }
};

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
        printf(screen, "Unknown command '%s'\n", argv[0]);
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
        printf(screen, "Unknown app '%s'\n", argv[1]);
        return;
    }
    user_spawn(app, rects[r].x,  rects[r].y, rects[r].w, rects[r].h);
}

void main(void) {
    struct screen screen;
    char line[128];
    unsigned int n;

    screen_init(&screen);
    screen_sync(&screen);
    putchar(&screen, '$'); putchar(&screen, ' ');
    for (;;) {
        char c = user_get();
        if (c == '\r') {
            putchar(&screen, '\n');
            line[n] = 0;
            exec(&screen, line);
            n = 0;
            putchar(&screen, '$');
            putchar(&screen, ' ');
        }
        else {
            putchar(&screen, c);
            if (n < sizeof(line) - 1) line[n++] = c;
        }
    }
}
