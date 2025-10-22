#include "syslib.h"
#include "screen.h"
#include "kb.h"

void kb_readline(struct screen *screen, char *line, int size) {
    int n = 0;
    for (;;) {
        char c = user_get();
        if (c == '\r') {
            screen_putchar(screen, '\n');
            line[n] = 0;
            return;
        }
        screen_putchar(screen, c);
        if (c == '\b' || c == '\177') { if (n > 0) n--; }
        else if (n < size - 1) line[n++] = c;
    }
}
