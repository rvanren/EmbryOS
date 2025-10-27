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

/* ----- drawing helpers (unchanged) ----- */
static void draw_cell(int r, int c, char ch) { user_put(r, c, CELL(ch, ANSI_BLACK, ANSI_YELLOW)); }
static void clear_cell(int r, int c)        { user_put(r, c, CELL(' ', ANSI_YELLOW, ANSI_YELLOW)); }
static void clear_screen(void){ for (int r=0;r<HEIGHT;r++) for (int c=0;c<WIDTH;c++) clear_cell(r,c); }
static int  idx_head(void) { return (tail + len - 1) % MAXLEN; }
static point_t get_at(int i){ return pos[(tail + i) % MAXLEN]; }

/* ----- init (unchanged) ----- */
static void init_snake(void){
    clear_screen();
    int mid = HEIGHT / 2;
    pos[0]=(point_t){mid,0}; pos[1]=(point_t){mid,1}; pos[2]=(point_t){mid,2};
    tail=0; len=3;
    draw_cell(pos[0].r,pos[0].c,'o');
    draw_cell(pos[1].r,pos[1].c,'o');
}

/* ----- collision / move (unchanged logic) ----- */
static int hits_body(int r,int c,int grow){
    for(int i=0;i<len;i++){
        if(!grow && i==0) continue; // tail vacates
        point_t p=get_at(i);
        if(p.r==r && p.c==c) return 1;
    }
    return 0;
}
static int would_be_blocked(int dr,int dc,int grow){
    point_t head=get_at(len-1);
    int nr=head.r+dr, nc=head.c+dc;
    if(nr<0||nr>=HEIGHT||nc<0||nc>=WIDTH) return 1;
    if(hits_body(nr,nc,grow)) return 1;
    return 0;
}
static void move_snake(int grow){
    point_t old_head=get_at(len-1);
    int nr=old_head.r+dir_r, nc=old_head.c+dir_c;

    if(!grow){
        point_t old_tail=pos[tail];
        clear_cell(old_tail.r,old_tail.c);
        tail=(tail+1)%MAXLEN;
    } else if(len<MAXLEN){
        len++;
    }

    int nh=(tail+len-1)%MAXLEN;
    pos[nh]=(point_t){nr,nc};

    draw_cell(old_head.r,old_head.c,'o'); // old head becomes body
}

/* ----- input: return 1 if a direction was produced; 0 otherwise; sets dr/dc or quit flag ----- */
static int read_direction_or_quit(int *dr,int *dc,int *quit){
    *quit=0;
    point_t head=pos[idx_head()];

    int key=user_get(head.r, head.c,
                     CELL('@', ANSI_BLUE,  ANSI_YELLOW),   // focused
                     CELL('X', ANSI_BLACK, ANSI_YELLOW));  // unfocused

    // vi keys
    if(key=='h'){ *dr=0;  *dc=-1; return 1; }
    if(key=='j'){ *dr=1;  *dc=0;  return 1; }
    if(key=='k'){ *dr=-1; *dc=0;  return 1; }
    if(key=='l'){ *dr=0;  *dc=1;  return 1; }

    // quit
    if(key=='q' || key=='Q'){ *quit=1; return 0; }

    // Arrow keys: ESC [ A/B/C/D   or   ESC O A/B/C/D
    if(key==27){
        // read the next byte
        int k1=user_get(head.r, head.c,
                        CELL('@', ANSI_BLUE,  ANSI_YELLOW),
                        CELL('X', ANSI_BLACK, ANSI_YELLOW));
        if(k1!='[' && k1!='O') return 0; // not an arrow sequence we recognize

        int k2=user_get(head.r, head.c,
                        CELL('@', ANSI_BLUE,  ANSI_YELLOW),
                        CELL('X', ANSI_BLACK, ANSI_YELLOW));
        switch(k2){
            case 'A': *dr=-1; *dc=0;  return 1; // up
            case 'B': *dr=1;  *dc=0;  return 1; // down
            case 'C': *dr=0;  *dc=1;  return 1; // right
            case 'D': *dr=0;  *dc=-1; return 1; // left
            default: return 0;
        }
    }

    // any other key: no direction
    return 0;
}

/* ----- main loop with "move only on valid direction" ----- */
int main(void){
    init_snake();

    while(1){
        int dr=dir_r, dc=dir_c, quit=0;
        int have_dir = read_direction_or_quit(&dr,&dc,&quit);
        if(quit){ user_exit(); return 0; }

        // Only proceed if we got a *new* direction command
        if(!have_dir){
            // No movement at all on non-direction keys
            continue;
        }

        // Prevent reversal
        if(len>1){
            point_t head=get_at(len-1), neck=get_at(len-2);
            int cur_dr=head.r-neck.r, cur_dc=head.c-neck.c;
            if(dr==-cur_dr && dc==-cur_dc){
                // Invalid reverse: do nothing
                continue;
            }
        }

        // Decide growth ONLY when we’re actually going to attempt a move
        int grow = ((moves + 1) % GROW_INTERVAL == 0);

        // If the requested direction is blocked, do nothing
        if(would_be_blocked(dr, dc, grow)){
            continue;
        }

        // Commit direction and move; count only successful moves
        dir_r=dr; dir_c=dc;
        move_snake(grow);
        moves++;
    }
}
