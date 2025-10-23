#include <stdarg.h>
#include "screen.h"
#include "stdio.h"

static struct screen screen;
static int initialized = 0;

void putchar(char c) {
    if (!initialized) {
        screen_init(&screen);
        screen_sync(&screen);
        initialized = 1;
    }
    screen_putchar(&screen, c);
}
