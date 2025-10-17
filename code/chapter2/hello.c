#include "screen.h"   // declare screen_* API

typedef unsigned int uint32_t;

struct uart { uint32_t txdata; };

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define TXFULL (1 << 31)

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}

/* crude busy-wait delay; adjust for speed */
static void delay(void) {
    for (volatile int i = 0; i < 10000000; i++)
        ;
}

int main(void) {
    const char *banner = "==== EmbryOS says Hello World ====   ";
    const int len   = 38;                 // length of banner string (include spaces)
    const int row   = 12;                 // vertical position (middle of 24-line screen)
    const int width = SCREEN_COLS;        // logical screen width
    int color = 1;
    int offset = 0;

    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 7, 0);

    while (1) {
        // draw one frame of the scrolling banner
        for (int i = 0; i < width; i++) {
            char c = banner[(i + offset) % len];
            screen_move(row, i);
            screen_put(c, color, 0);
        }

        // cycle color & offset for next frame
        color = (color % 7) + 1;
        offset = (offset + 1) % len;

        delay();
    }

    return 0;
}
