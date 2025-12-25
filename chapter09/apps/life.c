#include "syslib.h"
#include "blockpixel.h"

#define WIDTH       39
#define HEIGHT      22
#define ITERATIONS 250

#define COLOR_ALIVE     ANSI_GREEN
#define COLOR_DEAD      ANSI_YELLOW

struct cell { int x, y; };

static const struct cell gosper_gun[] = {
    {1,5}, {1,6}, {2,5}, {2,6}, {11,5}, {11,6}, {11,7}, {12,4},
    {12,8}, {13,3}, {13,9}, {14,3}, {14,9}, {15,6}, {16,4}, {16,8},
    {17,5}, {17,6}, {17,7}, {18,6}, {21,3}, {21,4}, {21,5}, {22,3},
    {22,4}, {22,5}, {23,2}, {23,6}, {25,1}, {25,2}, {25,6}, {25,7},
    {35,3}, {35,4}, {36,3}, {36,4},
};
static const int gosper_gun_cells = sizeof(gosper_gun) / sizeof(gosper_gun[0]);

struct life {
    int grid[HEIGHT][WIDTH];
    struct bp bp;
    uint8_t bp_buffer[WIDTH * HEIGHT];
};

static void clearGrid(struct life *life) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            life->grid[r][c] = 0;
}

static void initGosperGun(struct life *life, int offset_x, int offset_y) {
    clearGrid(life);
    for (int i = 0; i < gosper_gun_cells; i++) {
        int x = gosper_gun[i].x + offset_x;
        int y = gosper_gun[i].y + offset_y;
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            life->grid[y][x] = 1;
    }
}

static void drawGrid(struct life *life) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            bp_put(&life->bp, c, r, life->grid[r][c]
                    ?  COLOR_ALIVE : COLOR_DEAD, BP_LAZY);
    bp_flush(&life->bp);
}

static int countAliveNeighbours(const struct life *life, int r, int c) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int y = r + dy, x = c + dx;
            if (y < 0 || y >= HEIGHT) continue;
            if (x < 0 || x >= WIDTH)  continue;
            count += life->grid[y][x];
        }
    return count;
}

static void updateGrid(struct life *life) {
    int newGrid[HEIGHT][WIDTH];
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++) {
            int n = countAliveNeighbours(life, r, c);
            newGrid[r][c] = life->grid[r][c] ? (n == 2 || n == 3) : (n == 3);
        }
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            life->grid[r][c] = newGrid[r][c];
}

void main(void) {
    struct life life;
    bp_init(&life.bp, 0, 0, WIDTH, HEIGHT, life.bp_buffer);
    initGosperGun(&life, 0, 0);
    for (int t = 0; t < ITERATIONS; t++) {
        drawGrid(&life);
        updateGrid(&life);
        user_delay(100);
    }
}
