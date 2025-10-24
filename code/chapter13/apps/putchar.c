#include <stdarg.h>
#include "window.h"
#include "stdio.h"

static struct window window;
static int initialized = 0;

static void init(){
    if (!initialized) {
        window_init(&window);
        window_sync(&window);
        initialized = 1;
    }
}

void putchar(char c) {
    init();
    window_putchar(&window, c);
}

char getchar(void) {
    init();
    return window_getchar(&window);
}
