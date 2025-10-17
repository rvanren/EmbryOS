typedef unsigned int uint32_t;

struct uart { uint32_t txdata; };

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define TXFULL (1 << 31)

#define FRAMEBUFFER ((volatile unsigned int *) 0x40000000)
#define WIDTH   640
#define HEIGHT  480

void putchar(char c) {
    while (UART->txdata & TXFULL)
        ;
    UART->txdata = c;
}

void printf(char *str) {
    while (*str != 0)
        putchar(*str++);
}

void draw_screen(unsigned int color) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            FRAMEBUFFER[y * WIDTH + x] = color;
        }
    }
}

int main() {
    printf("Hello World\n");
    draw_screen(0x0000FF00);   // ARGB: 00RRGGBB --> bright green
}
