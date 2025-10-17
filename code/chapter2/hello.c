#include "stdio.h"

typedef unsigned int uint32_t;
struct uart { uint32_t txdata; };

// Memory-mapped UART
#define UART    ((struct uart *) 0x10010000)
#define TXFULL  (1 << 31)

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}

void clear_screen(void) {
    printf("\033[2J\033[H");
}

void set_cursor(int row, int col, int color) {
    printf("\033[%d;%dH\033[3%dm", row, col, color);
}

/* crude busy-wait delay; tune this constant for speed */
static void delay(void) {
    for (volatile int i = 0; i < 10000000; i++)
        ;
}

int main(void) {
    const char *banner = "==== EmbryOS says Hello World ====   ";
    const int width = 80, row = 12;   // assume 80x24 terminal
    int color = 1;

    int len = 0;	 // compute banner length
    while (banner[len] != '\0') len++;

    clear_screen();
    for (int offset = 0;;) {
        clear_screen();
        set_cursor(row, 1, color++);
        if (color > 7) color = 1;
        for (int i = 0; i < width; i++) {
            char c = banner[(i + offset) % len];
            putchar(c);
        }
        delay();
        offset = (offset + 1) % len;   // scroll left continuously
    }
    return 0;
}
