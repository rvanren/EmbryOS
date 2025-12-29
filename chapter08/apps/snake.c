#include "syslib.h"

#define WIDTH         39
#define HEIGHT        11
#define MAXLEN        (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

struct snake {
    point_t pos[MAXLEN];
    int tail, len, dir_r, dir_c, moves;
};

static void draw_cell(int r, int c, char ch) {
    user_put(c, r, CELL(ch, ANSI_BLACK, ANSI_YELLOW));
}

static void clear_cell(int r, int c) {
    user_put(c, r, CELL(' ', ANSI_YELLOW, ANSI_YELLOW));
}

static void clear_screen(void) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++) clear_cell(r, c);
}

static int idx_head(const struct snake *snake) {
    return (snake->tail + snake->len - 1) % MAXLEN;
}

static point_t get_at(const struct snake *snake, int i) {
    return snake->pos[(snake->tail + i) % MAXLEN];
}

static void init_snake(struct snake *snake) {
    clear_screen();

    int mid = HEIGHT / 2;
    snake->pos[0] = (point_t){mid, 0};
    snake->pos[1] = (point_t){mid, 1};
    snake->pos[2] = (point_t){mid, 2};
    snake->tail  = 0;
    snake->len   = 3;
    snake->dir_r = 0;
    snake->dir_c = 1;
    snake->moves = 0;

    draw_cell(snake->pos[0].r, snake->pos[0].c, 'o');
    draw_cell(snake->pos[1].r, snake->pos[1].c, 'o');
}

static int hits_body(const struct snake *snake, int r, int c, int grow) {
    for (int i = 0; i < snake->len; i++) {
        if (!grow && i == 0) continue;  // tail vacates
        point_t p = get_at(snake, i);
        if (p.r == r && p.c == c) return 1;
    }
    return 0;
}

static int would_be_blocked(const struct snake *snake, int dr, int dc, int grow) {
    point_t head = get_at(snake, snake->len - 1);
    int nr = head.r + dr;
    int nc = head.c + dc;
    if (nr < 0 || nr >= HEIGHT || nc < 0 || nc >= WIDTH) return 1;
    return hits_body(snake, nr, nc, grow);
}

static void move_snake(struct snake *snake, int grow) {
    point_t old_head = get_at(snake, snake->len - 1);
    int nr = old_head.r + snake->dir_r;
    int nc = old_head.c + snake->dir_c;

    if (!grow) {
        point_t old_tail = snake->pos[snake->tail];
        clear_cell(old_tail.r, old_tail.c);
        snake->tail = (snake->tail + 1) % MAXLEN;
    }
    else if (snake->len < MAXLEN) snake->len++;

    int nh = (snake->tail + snake->len - 1) % MAXLEN;
    snake->pos[nh] = (point_t){nr, nc};
    draw_cell(old_head.r, old_head.c, 'o');  // old head becomes body
}

static int read_direction_or_quit(struct snake *snake, int *dr, int *dc, int *quit) {
    *quit = 0;
    point_t head = snake->pos[idx_head(snake)];

    user_put(head.c, head.r, CELL('@', ANSI_BLUE,  ANSI_YELLOW));
    int key;
    for (;;) {
        key = user_get(1);
        if (key == USER_GET_GOT_FOCUS)
            user_put(head.c, head.r, CELL('@', ANSI_BLUE,  ANSI_YELLOW));
        else if (key == USER_GET_LOST_FOCUS)
            user_put(head.c, head.r, CELL('X', ANSI_BLACK, ANSI_YELLOW));
        else break;
    }

    // vi keys
    if (key == 'h' || key == 'a') { *dr = 0;  *dc = -1; return 1; }
    if (key == 'j' || key == 's') { *dr = 1;  *dc =  0; return 1; }
    if (key == 'k' || key == 'w') { *dr = -1; *dc =  0; return 1; }
    if (key == 'l' || key == 'd') { *dr = 0;  *dc =  1; return 1; }

    // quit
    if (key == 'q' || key == 'Q') { *quit = 1; return 0; }

    // arrow keys: ESC [ A/B/C/D  or  ESC O A/B/C/D
    if (key == 27) {
        int k1 = user_get(1);
        if (k1 != '[' && k1 != 'O') return 0;

        int k2 = user_get(1);
        switch (k2) {
            case 'A': *dr = -1; *dc =  0; return 1;  // up
            case 'B': *dr =  1; *dc =  0; return 1;  // down
            case 'C': *dr =  0; *dc =  1; return 1;  // right
            case 'D': *dr =  0; *dc = -1; return 1;  // left
            default:  return 0;
        }
    }

    return 0;
}

void main(void) {
    struct snake snake;
    init_snake(&snake);

    while (1) {
        int dr = snake.dir_r;
        int dc = snake.dir_c;
        int quit = 0;
        int have_dir = read_direction_or_quit(&snake, &dr, &dc, &quit);
        if (quit) break;
        if (!have_dir) continue;  // ignore non-direction keys

        // Prevent reversal
        if (snake.len > 1) {
            point_t head = get_at(&snake, snake.len - 1);
            point_t neck = get_at(&snake, snake.len - 2);
            int cur_dr = head.r - neck.r;
            int cur_dc = head.c - neck.c;
            if (dr == -cur_dr && dc == -cur_dc) continue;
        }

        int grow = ((snake.moves + 1) % GROW_INTERVAL == 0);
        if (would_be_blocked(&snake, dr, dc, grow)) continue;

        snake.dir_r = dr;
        snake.dir_c = dc;
        move_snake(&snake, grow);
        snake.moves++;
    }
}
