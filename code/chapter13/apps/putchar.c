#include <stdarg.h>
#include "window.h"
#include "stdio.h"

static struct window window;
static int initialized = 0;

void putchar(char c) {
    if (!initialized) {
        window_init(&window);
        window_sync(&window);
        initialized = 1;
    }
    window_putchar(&window, c);
}
