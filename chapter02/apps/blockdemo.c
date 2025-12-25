#include "syslib.h"
#include "blockpixel.h"

#define WIDTH   39
#define HEIGHT  22

int in_range(int x, int y) {
    x -= WIDTH / 2;
    y -= HEIGHT / 2;
    return (x * x) + (y * y) < 75;
}

void blockdemo_main(void) {
    struct bp bp;
    uint8_t buffer[WIDTH * HEIGHT];

    bp_init(&bp, 0, 0, WIDTH, HEIGHT, buffer);

    for (int i = 0;; i++) {
        int color = i;
        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++, color++)
                if (in_range(x, y))
                    bp_put(&bp, x, y, color % 8, BP_LAZY);
        bp_flush(&bp);
        user_delay(250);
    }
}
