#include "syslib.h"
#include "stdio.h"
#include "kb.h"

void kb_readline(char *line, int size) {
    int n = 0;
    for (;;) {
        char c = getchar();
        if (c == '\n') {
            line[n] = 0;
            return;
        }
        if (c == '\b' || c == '\177') { if (n > 0) n--; }
        else if (n < size - 1) line[n++] = c;
    }
}
