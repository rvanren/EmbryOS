#include "syslib.h"

#define WIDTH  39
#define HEIGHT 11

static void delay(void) {
    for (volatile int i = 0; i < 200000; i++);
}

static void clear(void) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            user_put(r, c, CELL(' ', ANSI_WHITE, ANSI_BLACK));
}

static void draw_centered(const char *s, int row) {
    int len = 0;
    while (s[len]) len++;
    int start = (WIDTH - len) / 2;
    for (int i = 0; i < len; i++)
        user_put(row, start + i, CELL(s[i], ANSI_WHITE, ANSI_BLACK));
}

void main(void) {
    const char *title = "EmbryOS";
    const char *wave[4] = {
        ".........................",
        "..........ooo............",
        ".......OOO***OOO.........",
        "..........ooo............"
    };

    clear();

    // Animate wave background
    for (int phase = 0; phase < 8; phase++) {
        for (int r = 0; r < HEIGHT; r++) {
            const char *pattern = wave[(r + phase) % 4];
            int len = 0; while (pattern[len]) len++;
            int start = (WIDTH - len) / 2;
            for (int i = 0; i < len; i++)
                user_put(r, start + i, CELL(pattern[i], ANSI_WHITE, ANSI_BLACK));
        }

        // Grow the title one letter at a time
        for (int i = 0; i < 7; i++) {
            draw_centered(title, HEIGHT / 2);
            delay();
        }

        delay();
        clear();
    }

    // Final static logo
    draw_centered("EmbryOS", HEIGHT / 2);
    draw_centered("ready.", HEIGHT / 2 + 2);
}
