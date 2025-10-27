#include "syslib.h"

#define WIDTH         39
#define HEIGHT        11
#define MAXLEN        (WIDTH * HEIGHT)
#define GROW_INTERVAL 3

typedef struct { int r, c; } point_t;

struct snake {
    point_t pos[MAXLEN];
    int     tail;
    int     len;
    int     dir_r, dir_c;
    int     moves;
};

/* -------------------------------------------------------- */
/* Drawing helpers                                           */
/* -------------------------------------------------------- */
static void draw_cell(int r, int c, char ch) {
    user_put(r, c, CELL(ch, ANSI_BLACK, ANSI_YELLOW));
}

static void clear_cell(int r, int c) {
    user_put(r, c, CELL(' ', ANSI_YELLOW, ANSI_YELLOW));
}

static void clear_screen(void) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            clear_cell(r, c);
}

static int idx_head(const struct snake *S) {
    return (S->tail + S->len - 1) % MAXLEN;
}

static point_t get_at(const struct snake *S, int i) {
    return S->pos[(S->tail + i) % MAXLEN];
}

/* -------------------------------------------------------- */
/* Initialization                                            */
/* -------------------------------------------------------- */
static void init_snake(struct snake *S) {
    clear_screen();

    int mid = HEIGHT / 2;
    S->pos[0] = (point_t){mid, 0};
    S->pos[1] = (point_t){mid, 1};
    S->pos[2] = (point_t){mid, 2};

    S->tail  = 0;
    S->len   = 3;
    S->dir_r = 0;
    S->dir_c = 1;
    S->moves = 0;

    draw_cell(S->pos[0].r, S->pos[0].c, 'o');
    draw_cell(S->pos[1].r, S->pos[1].c, 'o');
}

/* -------------------------------------------------------- */
/* Collision and movement                                   */
/* -------------------------------------------------------- */
static int hits_body(const struct snake *S, int r, int c, int grow) {
    for (int i = 0; i < S->len; i++) {
        if (!grow && i == 0) continue;  // tail vacates
        point_t p = get_at(S, i);
        if (p.r == r && p.c == c) return 1;
    }
    return 0;
}

static int would_be_blocked(const struct snake *S, int dr, int dc, int grow) {
    point_t head = get_at(S, S->len - 1);
    int nr = head.r + dr;
    int nc = head.c + dc;

    if (nr < 0 || nr >= HEIGHT || nc < 0 || nc >= WIDTH)
        return 1;

    if (hits_body(S, nr, nc, grow))
        return 1;

    return 0;
}

static void move_snake(struct snake *S, int grow) {
    point_t old_head = get_at(S, S->len - 1);
    int nr = old_head.r + S->dir_r;
    int nc = old_head.c + S->dir_c;

    if (!grow) {
        point_t old_tail = S->pos[S->tail];
        clear_cell(old_tail.r, old_tail.c);
        S->tail = (S->tail + 1) % MAXLEN;
    } else if (S->len < MAXLEN) {
        S->len++;
    }

    int nh = (S->tail + S->len - 1) % MAXLEN;
    S->pos[nh] = (point_t){nr, nc};

    draw_cell(old_head.r, old_head.c, 'o');  // old head becomes body
}

/* -------------------------------------------------------- */
/* Input handling                                            */
/* -------------------------------------------------------- */
static int read_direction_or_quit(struct snake *S, int *dr, int *dc, int *quit) {
    *quit = 0;

    point_t head = S->pos[idx_head(S)];

    int key = user_get(head.r, head.c,
                       CELL('@', ANSI_BLUE,  ANSI_YELLOW),
                       CELL('X', ANSI_BLACK, ANSI_YELLOW));

    // vi keys
    if (key == 'h') { *dr = 0;  *dc = -1; return 1; }
    if (key == 'j') { *dr = 1;  *dc =  0; return 1; }
    if (key == 'k') { *dr = -1; *dc =  0; return 1; }
    if (key == 'l') { *dr = 0;  *dc =  1; return 1; }

    // quit
    if (key == 'q' || key == 'Q') { *quit = 1; return 0; }

    // Arrow keys: ESC [ A/B/C/D  or  ESC O A/B/C/D
    if (key == 27) {
        int k1 = user_get(head.r, head.c,
                          CELL('@', ANSI_BLUE,  ANSI_YELLOW),
                          CELL('X', ANSI_BLACK, ANSI_YELLOW));
        if (k1 != '[' && k1 != 'O') return 0;

        int k2 = user_get(head.r, head.c,
                          CELL('@', ANSI_BLUE,  ANSI_YELLOW),
                          CELL('X', ANSI_BLACK, ANSI_YELLOW));
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

/* -------------------------------------------------------- */
/* Main loop                                                 */
/* -------------------------------------------------------- */
void snake_main(void) {
    struct snake S;
    init_snake(&S);

    while (1) {
        int dr = S.dir_r;
        int dc = S.dir_c;
        int quit = 0;

        int have_dir = read_direction_or_quit(&S, &dr, &dc, &quit);
        if (quit) user_exit();

        if (!have_dir)
            continue;  // ignore non-direction keys

        // Prevent reversal
        if (S.len > 1) {
            point_t head = get_at(&S, S.len - 1);
            point_t neck = get_at(&S, S.len - 2);
            int cur_dr = head.r - neck.r;
            int cur_dc = head.c - neck.c;

            if (dr == -cur_dr && dc == -cur_dc)
                continue;
        }

        int grow = ((S.moves + 1) % GROW_INTERVAL == 0);

        if (would_be_blocked(&S, dr, dc, grow))
            continue;

        S.dir_r = dr;
        S.dir_c = dc;
        move_snake(&S, grow);
        S.moves++;
    }
}
