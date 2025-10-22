#define NROWS 12
#define NCOLS 40

struct cell { char c, fg, bg; };
struct screen {
    struct cell cells[NROWS][NCOLS];
    int cur_col, cur_fg, cur_bg;
};

void screen_init(struct screen *screen);
void screen_sync(struct screen *screen);
void screen_scroll(struct screen *screen);
void screen_putchar(struct screen *screen, char c);
