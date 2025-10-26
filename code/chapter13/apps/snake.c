#include "syslib.h"

#define WIDTH   39
#define HEIGHT  11
#define MAXLEN  (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

static point_t pos[MAXLEN];  // ring buffer of segments (tail..head)
static int tail = 0;         // index of current tail in pos[]
static int len  = 3;         // number of segments
static int dir_r = 0, dir_c = 1; // moving right
static int moves = 0;

static void draw_cell(int r, int c, char ch) {
    user_put(r, c, CELL(ch, ANSI_GREEN, ANSI_BLACK));
}
static void clear_cell(int r, int c) {
    user_put(r, c, CELL(' ', ANSI_BLACK, ANSI_BLACK));
}

static int idx_head(void) {
    return (tail + len - 1) % MAXLEN;
}
static point_t get_at(int i) {
    return pos[(tail + i) % MAXLEN];
}

static void init_snake(void) {
    int mid = HEIGHT / 2;
    pos[0] = (point_t){mid, 0};
    pos[1] = (point_t){mid, 1};
    pos[2] = (point_t){mid, 2};
    tail = 0; len = 3;
    // draw initial body (head is drawn by user_get)
    draw_cell(pos[0].r, pos[0].c, 'o');
    draw_cell(pos[1].r, pos[1].c, 'o');
}

// Return 1 if (r,c) collides with body.
// If !grow, stepping onto the current tail is allowed (it will move away).
static int hits_body(int r, int c, int grow) {
    for (int i = 0; i < len; i++) {
        if (!grow && i == 0) continue; // allow stepping into tail if not growing
        point_t p = get_at(i);
        if (p.r == r && p.c == c) return 1;
    }
    return 0;
}

static void move_snake(int grow) {
    int h = idx_head();
    point_t old_head = pos[h];
    int new_r = old_head.r + dir_r;
    int new_c = old_head.c + dir_c;

    // bounds
    if (new_r < 0 || new_r >= HEIGHT || new_c < 0 || new_c >= WIDTH)
        return;

    // collision (with tail exception on non-grow)
    if (hits_body(new_r, new_c, grow))
        return;

    // normal move: pop/clear tail
    if (!grow) {
        point_t old_tail = pos[tail];
        clear_cell(old_tail.r, old_tail.c);
        tail = (tail + 1) % MAXLEN;
    } else if (len < MAXLEN) {
        len++; // extend snake: we do not remove the tail this step
    }

    // write new head
    int new_head_idx = (tail + len - 1) % MAXLEN;
    pos[new_head_idx] = (point_t){new_r, new_c};

    // redraw previous head as body segment
    draw_cell(old_head.r, old_head.c, 'o');
}

int main(void) {
    init_snake();

    while (1) {
        // decide growth BEFORE moving: every third move
        int grow = ((moves + 1) % GROW_INTERVAL == 0);

        // draw focus-aware head and block for input
        point_t head = pos[idx_head()];
        int key = user_get(
            head.r, head.c,
            CELL('@', ANSI_YELLOW, ANSI_BLACK), // focused
            CELL('@', ANSI_BLUE,   ANSI_BLACK)  // unfocused
        );

        int new_dr = dir_r, new_dc = dir_c;
        switch (key) {
            case 'k': new_dr = -1; new_dc =  0; break; // up
            case 'j': new_dr =  1; new_dc =  0; break; // down
            case 'l': new_dr =  0; new_dc =  1; break; // right
            case 'h': new_dr =  0; new_dc = -1; break; // left
            case 'q':
            case 'Q':
                user_exit();
                return 0;
            default: break;
        }

        // prevent direct reversal (compare to neck vector)
        if (len > 1) {
            point_t neck = get_at(len - 2);
            int cur_dr = head.r - neck.r;
            int cur_dc = head.c - neck.c;
            if (new_dr == -cur_dr && new_dc == -cur_dc) {
                new_dr = cur_dr;
                new_dc = cur_dc;
            }
        }

        dir_r = new_dr;
        dir_c = new_dc;

        move_snake(grow);
        moves++;
    }
}
