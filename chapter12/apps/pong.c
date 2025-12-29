#include "syslib.h"
#include "blockpixel.h"

#define WIDTH       39
#define HEIGHT      20

#define PADDLE_POS   (WIDTH - 4)
#define PADDLE_SIZE  4

struct pong {
    int paddle;     // vertical paddle position
    int x, y;       // ball position
    int dx, dy;     // ball direction
    struct bp bp;
    uint8_t bp_buffer[WIDTH * HEIGHT];
};

void pong_redraw(struct pong *pong) {
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            bp_put(&pong->bp, x, y, ANSI_GREEN, BP_LAZY);
    for (int y = pong->paddle - PADDLE_SIZE / 2; y < pong->paddle + PADDLE_SIZE / 2; y++)
        bp_put(&pong->bp, PADDLE_POS, y, ANSI_YELLOW, BP_LAZY);
    bp_put(&pong->bp, pong->x, pong->y, ANSI_RED, BP_LAZY);
    bp_flush(&pong->bp);
}

void print_score(int x, int score) {
    if (score == 0) {
        user_put(x, 0, CELL('0', ANSI_WHITE, ANSI_BLACK));
        return;
    }
    while (score != 0) {
        user_put(x, 0, CELL('0' + score % 10, ANSI_WHITE, ANSI_BLACK));
        score /= 10;
        x--;
    }
}

void main(void) {
    int delay = 50, t = 0;
    int computer = 0, human = 0;
    struct pong pong;

    pong.paddle = HEIGHT / 2;
    pong.x = WIDTH / 2;
    pong.y = HEIGHT / 2;
    pong.dx = 1;
    pong.dy = 1;
    bp_init(&pong.bp, 0, 1, WIDTH, HEIGHT, pong.bp_buffer);
    pong_redraw(&pong);
    for (int x = 0; x < WIDTH; x++)
        user_put(x, 0, CELL(' ', ANSI_WHITE, ANSI_BLACK));
    for (;;) {
        print_score(WIDTH / 3, computer);
        print_score(WIDTH * 2 / 3, human);
        int c = user_get(0);
        while (c == USER_GET_LOST_FOCUS) c = user_get(1);
        if (c == 'q') break;
        if (c == 'w' || c == 'k') pong.paddle--;
        else if (c == 's' || c == 'j') pong.paddle++;
        else if (c == 27) { // ESC
            c = user_get(1);
            if (c != '[' && c != 'O') continue;
            c = user_get(1);
            switch (c) {
                case 'A': pong.paddle--; break;
                case 'B': pong.paddle++; break;
            }
        }
        else {
            if (++t >= delay) {
                if (pong.dx > 0 && pong.x + pong.dx == PADDLE_POS &&
                            pong.paddle - PADDLE_SIZE / 2 <= pong.y &&
                            pong.y < pong.paddle + PADDLE_SIZE / 2) {
                    pong.dx = -pong.dx;
                    human++;
                    if (delay > 20) delay--;
                }
                if (pong.x == 0 && pong.dx == -1) {
                    pong.dx = 1;
                }
                if (pong.x == WIDTH - 1 && pong.dx == 1) {
                    pong.dx = -1;
                    if (delay > 20) delay--;
                    computer++;
                }
                if (pong.y == 0 && pong.dy == -1) {
                    pong.dy = 1;
                }
                if (pong.y == HEIGHT - 1 && pong.dy == 1) {
                    pong.dy = -1;
                }
                pong.x += pong.dx;
                pong.y += pong.dy;
                t = 0;
            }
        }
        pong_redraw(&pong);
    }
}
