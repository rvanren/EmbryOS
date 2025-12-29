#include "syslib.h"

#define WIDTH   39
#define HEIGHT  11

static void clear(void) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            user_put(c, r, CELL(' ', ANSI_WHITE, ANSI_BLACK));
}

static void put_centered(const char *s, int row) {
    int len = 0; while (s[len]) len++;
    if (row < 0 || row >= HEIGHT) return;
    int start = (WIDTH - len) / 2;
    if (start < 0) start = 0;
    for (int i = 0; i < len && start + i < WIDTH; i++)
        user_put(start + i, row, CELL(s[i], ANSI_WHITE, ANSI_BLACK));
}

void main(void) {
    clear();

    const char *title = "EmbryOS";
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;

    // expanding / contracting rings
    for (int phase = 0; phase < 12; phase++) {
        clear();

        int radius = (phase <= 6) ? phase : 12 - phase;

        for (int r = 0; r < HEIGHT; r++) {
            for (int c = 0; c < WIDTH; c++) {
                int dr = r - cy;
                int dc = c - cx;
                int dist = dr*dr + dc*dc;
                // crude ring: show '.' near the chosen radius^2
                if (dist >= (radius*radius - 2) && dist <= (radius*radius + 2))
                    user_put(c, r, CELL('.', ANSI_WHITE, ANSI_BLACK));
            }
        }

        // draw the logo in the middle
        put_centered(title, cy);
        user_delay(200);
    }

    // final stable screen
    clear();
    put_centered("EmbryOS", HEIGHT / 2 - 1);
    put_centered("Life Has Begun", HEIGHT / 2 + 1);
}
