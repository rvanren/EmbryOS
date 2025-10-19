#include "stdio.h"
#include "screen.h"
#include "uart.h"

void taskA(void) {
    const char *banner = "==== EmbryOS says Hello World ====   ";
    const int len   = 38;                 // length of banner string (include spaces)
    const int row   = 12;                 // vertical position (middle of 24-line screen)
    const int width = SCREEN_COLS;        // logical screen width
    int color = 1;
    int offset = 0;

    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 4);

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

        // delay
        for (volatile int i = 0; i < 10000000; i++) ;
    }
}

int main(void) {
    uart_init();
    taskA();
}
