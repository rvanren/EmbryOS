#include "syslib.h"
#include "stdio.h"
#include "kb.h"

void kb_readline(char *line, int size) {
    int n = 0;
    for (;;) {
        char c = user_get();
        if (c == '\r') {
            putchar('\n');
            line[n] = 0;
            return;
        }
        putchar(c);
        if (c == '\b' || c == '\177') { if (n > 0) n--; }
        else if (n < size - 1) line[n++] = c;
    }
}
