#include "syslib.h"
#include "blockpixel.h"
#include "thread.h"

#define WIDTH       39
#define HEIGHT      20

#define PADDLE_POS   (WIDTH - 4)
#define PADDLE_SIZE  4

struct pong {
    struct sema *mutex;
    int paddle;          // vertical paddle position
    int x, y;            // ball position
    int dx, dy;          // ball direction
    int focus;           // has focus
    int computer, human; // scores
    struct bp bp;
    uint8_t bp_buffer[WIDTH * HEIGHT];
};

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

void pong_redraw(struct pong *pong) {
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            bp_put(&pong->bp, x, y, ANSI_GREEN, BP_LAZY);
    for (int y = pong->paddle - PADDLE_SIZE / 2; y < pong->paddle + PADDLE_SIZE / 2; y++)
        bp_put(&pong->bp, PADDLE_POS, y, pong->focus ? ANSI_YELLOW : ANSI_BLUE, BP_LAZY);
    bp_put(&pong->bp, pong->x, pong->y, pong->focus ? ANSI_RED : ANSI_BLUE, BP_LAZY);
    bp_flush(&pong->bp);
    for (int x = 0; x < WIDTH; x++)
        user_put(x, 0, CELL(' ', ANSI_BLACK, ANSI_BLACK));
    print_score(WIDTH / 3, pong->computer);
    print_score(WIDTH * 2 / 3, pong->human);
}

void paddle_thread(void *arg) {
    struct pong *pong = arg;

    for (;;) {
        int delta = 0, new_focus = -1;
        int c = thread_get(); // Blocks until a key is pressed
        switch (c) {
        case 'q': user_exit(); break;
        case 'w': case 'k': delta = -1; break;
        case 's': case 'j': delta =  1; break;
        case 27: // ESC
            c = thread_get();
            if (c != '[' && c != 'O') continue;
            c = thread_get();
            switch (c) {
                case 'A': delta = -1; break;
                case 'B': delta =  1; break;
            }
        case USER_GET_GOT_FOCUS:  new_focus = 1; break;
        case USER_GET_LOST_FOCUS: new_focus = 0; break;
        }
        if (delta != 0 || new_focus != -1) {
            sema_dec(pong->mutex);
            pong->paddle += delta;
            if (new_focus != -1) pong->focus = new_focus;
            pong_redraw(pong);
            sema_inc(pong->mutex);
        }
    }
}

void ball_thread(void *arg) {
    struct pong *pong = arg;

    int delay = 50;     // milliseconds
    for (;;) {
        thread_sleep(user_gettime() + delay * 1000000ULL);
        sema_dec(pong->mutex);
        if (pong->dx > 0 && pong->x + pong->dx == PADDLE_POS &&
                    pong->paddle - PADDLE_SIZE / 2 <= pong->y &&
                    pong->y < pong->paddle + PADDLE_SIZE / 2) {
            pong->dx = -pong->dx;
            pong->human++;
            if (delay > 20) delay--;
        }
        if (pong->x == 0 && pong->dx == -1) {
            pong->dx = 1;
        }
        if (pong->x == WIDTH - 1 && pong->dx == 1) {
            pong->dx = -1;
            if (delay > 20) delay--;
            pong->computer++;
        }
        if (pong->y == 0 && pong->dy == -1) {
            pong->dy = 1;
        }
        if (pong->y == HEIGHT - 1 && pong->dy == 1) {
            pong->dy = -1;
        }
        pong->x += pong->dx;
        pong->y += pong->dy;

        pong_redraw(pong);
        sema_inc(pong->mutex);
    }
}

void main() {
    thread_init();
    struct pong pong;

    pong.mutex = sema_create(1);
    pong.paddle = HEIGHT / 2;
    pong.x = WIDTH / 2;
    pong.y = HEIGHT / 2;
    pong.dx = 1;
    pong.dy = 1;
    pong.focus = 1;
    pong.computer = 0;
    pong.human = 0;
    bp_init(&pong.bp, 0, 1, WIDTH, HEIGHT, pong.bp_buffer);
    pong_redraw(&pong);

    thread_create(ball_thread, &pong, 16 * 1024);
    thread_create(paddle_thread, &pong, 16 * 1024);
    thread_exit();
}
