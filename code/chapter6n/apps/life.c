#include "syslib.h"

#define WIDTH     39
#define HEIGHT    11
#define ITERATION 200
#define DELAY     50000000   // adjust for animation speed

struct cell { int x, y; };

// Gosper Glider Gun pattern (fits 39×11)
static const struct cell gosper_gun[] = {
    {1,5}, {1,6}, {2,5}, {2,6}, {11,5}, {11,6}, {11,7}, {12,4},
    {12,8}, {13,3}, {13,9}, {14,3}, {14,9}, {15,6}, {16,4}, {16,8},
    {17,5}, {17,6}, {17,7}, {18,6}, {21,3}, {21,4}, {21,5}, {22,3},
    {22,4}, {22,5}, {23,2}, {23,6}, {25,1}, {25,2}, {25,6}, {25,7},
    {35,3}, {35,4}, {36,3}, {36,4},
};
static const int gosper_gun_cells = sizeof(gosper_gun) / sizeof(gosper_gun[0]);

//----------------------------------------------
// Encapsulated state
//----------------------------------------------
struct life {
    int grid[HEIGHT][WIDTH];
};

//----------------------------------------------
// Operations
//----------------------------------------------
static void clearGrid(struct life *L) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            L->grid[r][c] = 0;
}

static void initGosperGun(struct life *L, int offset_x, int offset_y) {
    clearGrid(L);
    for (int i = 0; i < gosper_gun_cells; i++) {
        int x = gosper_gun[i].x + offset_x;
        int y = gosper_gun[i].y + offset_y;
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
            L->grid[y][x] = 1;
    }
}

static void drawGrid(const struct life *L) {
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            user_put(r, c, CELL(' ', ANSI_WHITE,
                    L->grid[r][c] ? ANSI_YELLOW : ANSI_GREEN));
}

static int countAliveNeighbours(const struct life *L, int r, int c) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int y = (r + dy + HEIGHT) % HEIGHT;
            int x = (c + dx + WIDTH) % WIDTH;
            count += L->grid[y][x];
        }
    return count;
}

static void updateGrid(struct life *L) {
    int newGrid[HEIGHT][WIDTH];
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++) {
            int n = countAliveNeighbours(L, r, c);
            newGrid[r][c] = L->grid[r][c] ? (n == 2 || n == 3) : (n == 3);
        }
    for (int r = 0; r < HEIGHT; r++)
        for (int c = 0; c < WIDTH; c++)
            L->grid[r][c] = newGrid[r][c];
}

//----------------------------------------------
// Main
//----------------------------------------------
void life_main(void) {
    struct life L;
    initGosperGun(&L, 0, 0);
    for (int t = 0; t < ITERATION; t++) {
        drawGrid(&L);
        updateGrid(&L);
        for (volatile int i = 0; i < DELAY; i++) ;
    }
}

#ifdef CH8
void main() { life_main(); }
#endif
