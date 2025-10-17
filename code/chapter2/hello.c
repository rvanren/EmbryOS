#include "stdio.h"

typedef unsigned int uint32_t;

struct uart { uint32_t txdata; };

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define TXFULL (1 << 31)

#define ESC "\033"

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}

void clear_screen(void) {
    printf(ESC "[2J");       // clear screen
    printf(ESC "[H");        // move cursor home
}

void move_cursor(int row, int col) {
    printf(ESC "[%d;%dH", row, col);
}

void set_color(int color) {
    printf(ESC "[3%dm", color);  // 0=black ... 7=white
}

int main(void) {
    clear_screen();
    for (int i = 0; i < 6; i++) {
        set_color(i + 1);
        move_cursor(2 + i, 10);
        printf("EmbryOS says hello!\n");
    }
    set_color(7);
    move_cursor(10, 0);
    printf("\n");
    return 0;
}
