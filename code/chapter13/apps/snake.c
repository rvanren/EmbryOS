#include "syslib.h"

#define WIDTH   39
#define HEIGHT  11
#define MAXLEN  (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

static point_t snake[MAXLEN];
static int length = 3;           // initial: oo@
static int dir_r = 0, dir_c = 1; // start moving right
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

// Advances snake one step in current direction
static void move_snake(void) {
    point_t head = snake[length - 1];
    int new_r = head.r + dir_r;
    int new_c = head.c + dir_c;

    // stop at borders
    if (new_r < 0 || new_r >= HEIGHT || new_c < 0 || new_c >= WIDTH)
        return;

    int grow = (moves % GROW_INTERVAL == GROW_INTERVAL - 1 && length < MAXLEN);

    if (!grow) {
        point_t tail = snake[0];
        clear_cell(tail.r, tail.c);
        for (int i = 0; i < length - 1; i++)
            snake[i] = snake[i + 1];
    } else {
        if (length < MAXLEN) length++;
        for (int i = 0; i < length - 1; i++)
            snake[i] = snake[i + 1];
    }

    snake[length - 1] = (point_t){new_r, new_c};

    for (int i = 0; i < length - 1; i++)
        draw_cell(snake[i].r, snake[i].c, 'o');
}

int main(void) {
    init_snake();

    while (1) {
        // Show the head and block for a keypress
        int key = user_get(
            snake[length - 1].r,
            snake[length - 1].c,
            CELL('@', ANSI_YELLOW, ANSI_BLACK), // focused
            CELL('@', ANSI_BLUE,   ANSI_BLACK)  // unfocused
        );

        switch (key) {
            case 'u': dir_r = -1; dir_c =  0; break; // up
            case 'd': dir_r =  1; dir_c =  0; break; // down
            case 'r': dir_r =  0; dir_c =  1; break; // right
            case 'l': dir_r =  0; dir_c = -1; break; // left
            case 'q': return;
        }

        move_snake();
        moves++;
    }
}
