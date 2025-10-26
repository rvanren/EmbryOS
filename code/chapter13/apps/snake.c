#include "syslib.h"

#define WIDTH   39
#define HEIGHT  11
#define MAXLEN  (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

static point_t snake[MAXLEN];
static int length = 3;
static int dir_r = 0, dir_c = 1;
static int moves = 0;

static void draw_cell(int r, int c, char ch) {
    user_put(r, c, CELL(ch, ANSI_GREEN, ANSI_BLACK));
}

static void clear_cell(int r, int c) {
    user_put(r, c, CELL(' ', ANSI_BLACK, ANSI_BLACK));
}

static void init_snake(void) {
    int mid = HEIGHT / 2;
    snake[0] = (point_t){mid, 0};
    snake[1] = (point_t){mid, 1};
    snake[2] = (point_t){mid, 2};
    for (int i = 0; i < length - 1; i++)
        draw_cell(snake[i].r, snake[i].c, 'o');
}

static int hits_body(int r, int c) {
    for (int i = 0; i < length; i++)
        if (snake[i].r == r && snake[i].c == c)
            return 1;
    return 0;
}

static void move_snake(void) {
    point_t head = snake[length - 1];
    int new_r = head.r + dir_r;
    int new_c = head.c + dir_c;

    // stop at borders or collisions
    if (new_r < 0 || new_r >= HEIGHT || new_c < 0 || new_c >= WIDTH)
        return;
    if (hits_body(new_r, new_c))
        return;

    int grow = (moves % GROW_INTERVAL == GROW_INTERVAL - 1 && length < MAXLEN);

    if (grow) {
        // grow: shift everything one step to the RIGHT
        for (int i = length; i > 0; i--)
            snake[i - 1] = snake[i - 1]; // dummy line for clarity
        for (int i = length; i > 0; i--)
            snake[i - 1] = snake[i - 1]; // (we’ll actually shift below)
        for (int i = length; i > 0; i--)
            snake[i - 1] = snake[i - 1]; // (ignore redundant clarity)
        // the real shift:
        for (int i = length; i > 0; i--)
            snake[i] = snake[i - 1];
        length++;
    } else {
        // normal move: erase tail, shift LEFT
        clear_cell(snake[0].r, snake[0].c);
        for (int i = 0; i < length - 1; i++)
            snake[i] = snake[i + 1];
    }

    // new head
    snake[length - 1] = (point_t){new_r, new_c};

    // redraw body
    for (int i = 0; i < length - 1; i++)
        draw_cell(snake[i].r, snake[i].c, 'o');
}

int main(void) {
    init_snake();

    while (1) {
        int key = user_get(
            snake[length - 1].r,
            snake[length - 1].c,
            CELL('@', ANSI_YELLOW, ANSI_BLACK),
            CELL('@', ANSI_BLUE, ANSI_BLACK)
        );

        int new_dr = dir_r, new_dc = dir_c;
        switch (key) {
            case 'k': new_dr = -1; new_dc =  0; break;
            case 'j': new_dr =  1; new_dc =  0; break;
            case 'l': new_dr =  0; new_dc =  1; break;
            case 'h': new_dr =  0; new_dc = -1; break;
            case 'q':
            case 'Q':
                user_exit();
                return 0;
            default:
                break;
        }

        // prevent reversing into itself
        if (length > 1) {
            point_t head = snake[length - 1];
            point_t neck = snake[length - 2];
            int cur_dr = head.r - neck.r;
            int cur_dc = head.c - neck.c;
            if (new_dr == -cur_dr && new_dc == -cur_dc) {
                new_dr = cur_dr;
                new_dc = cur_dc;
            }
        }

        dir_r = new_dr;
        dir_c = new_dc;

        move_snake();
        moves++;
    }
}
