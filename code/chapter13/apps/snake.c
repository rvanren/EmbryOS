#include "syslib.h"

#define WIDTH   39
#define HEIGHT  11
#define MAXLEN  (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

static point_t pos[MAXLEN];
static int tail = 0;
static int len  = 3;
static int dir_r = 0, dir_c = 1;
static int moves = 0;

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

static void draw_cell(int r, int c, char ch) {
    user_put(r, c, CELL(ch, ANSI_GREEN, ANSI_YELLOW));
}

static void clear_cell(int r, int c) {
    user_put(r, c, CELL(' ', ANSI_YELLOW, ANSI_YELLOW));
}

static void clear_screen(void) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            clear_cell(r, c);
}

static int idx_head(void) {
    return (tail + len - 1) % MAXLEN;
}

static point_t get_at(int i) {
    return pos[(tail + i) % MAXLEN];
}

// ----------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------

static void init_snake(void) {
    clear_screen();

    int mid = HEIGHT / 2;
    pos[0] = (point_t){mid, 0};
    pos[1] = (point_t){mid, 1};
    pos[2] = (point_t){mid, 2};
    tail = 0; len = 3;

    draw_cell(pos[0].r, pos[0].c, 'o');
    draw_cell(pos[1].r, pos[1].c, 'o');
}

// ----------------------------------------------------------------------
// Movement — ignore blocked directions
// ----------------------------------------------------------------------

static int hits_body(int r, int c, int grow) {
    for (int i = 0; i < len; i++) {
        if (!grow && i == 0) continue; // tail moves away
        point_t p = get_at(i);
        if (p.r == r && p.c == c) return 1;
    }
    return 0;
}

static void try_move_snake(int grow) {
    int h = idx_head();
    point_t old_head = pos[h];
    int new_r = old_head.r + dir_r;
    int new_c = old_head.c + dir_c;

    // check bounds
    if (new_r < 0 || new_r >= HEIGHT || new_c < 0 || new_c >= WIDTH)
        return; // can’t move in this direction
    // check self-collision
    if (hits_body(new_r, new_c, grow))
        return; // blocked by own body

    // normal move
    if (!grow) {
        point_t old_tail = pos[tail];
        clear_cell(old_tail.r, old_tail.c);
        tail = (tail + 1) % MAXLEN;
    } else if (len < MAXLEN) {
        len++;
    }

    int new_head_idx = (tail + len - 1) % MAXLEN;
    pos[new_head_idx] = (point_t){new_r, new_c};

    // redraw previous head as body
    draw_cell(old_head.r, old_head.c, 'o');
}

// ----------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------

int main(void) {
    init_snake();

    while (1) {
        int grow = ((moves + 1) % GROW_INTERVAL == 0);

        point_t head = pos[idx_head()];
        int key = user_get(
            head.r, head.c,
            CELL('@', ANSI_BLACK, ANSI_YELLOW), // focused
            CELL('@', ANSI_BLUE,  ANSI_YELLOW)  // unfocused
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
            default:
                break;
        }

        // prevent direct reversal
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

        try_move_snake(grow);
        moves++;
    }
}
