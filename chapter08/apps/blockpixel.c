#include "syslib.h"
#include "blockpixel.h"

#define BP_COLOR_MASK         0x7
#define BP_DIRTY_MASK         0x8
#define BP_GET_COLOR(b)       ((b) & BP_COLOR_MASK)
#define BP_IS_DIRTY(b)        ((b) & BP_DIRTY_MASK)
#define BP_SET_DIRTY(b)       ((b) |=  BP_DIRTY_MASK)
#define BP_CLEAR_DIRTY(b)     ((b) &= ~BP_DIRTY_MASK)
#define BP_MAKE(color, dirty) (((color)&7) | ((dirty)?BP_DIRTY_MASK:0))

static inline int idx(struct bp *bp, int px, int py) {
    return py * bp->width + px;
}

static inline void bp_render_pair(uint8_t top_color, uint8_t bottom_color,
                                  int *code, int *fg, int *bg) {
    if (top_color == bottom_color) { // Entire cell drawn using background
        *code = 0; // space with bg filling the full cell
        *fg = top_color; *bg = top_color;
    }
    else { // Mixed colors: choose canonical representation
        *code = 1; // top pixel = fg, bottom pixel = bg
        *fg = top_color; *bg = bottom_color;
    }
}

void bp_init(struct bp *bp, int x, int y, int width, int height,
                                 uint8_t *buffer) {
    bp->x = x; bp->y = y; bp->width = width; bp->height = height;
    bp->buf = buffer;
    for (int i = 0; i < width * height; i++) bp->buf[i] = BP_MAKE(0, 1);
}

void bp_put(struct bp *bp, int px, int py, uint8_t color, enum bp_mode mode) {
    if (px < 0 || px >= bp->width) return;
    if (py < 0 || py >= bp->height) return;
    bp->buf[idx(bp, px, py)] = BP_MAKE(color, 1);
    if (mode == BP_LAZY) return;
    int cell_x = px, cell_y = py / 2;
    int top_index    = (cell_y * 2) * bp->width + cell_x;
    int bottom_index = top_index + bp->width;
    uint8_t top_color    = BP_GET_COLOR(bp->buf[top_index]);
    uint8_t bottom_color = BP_GET_COLOR(bp->buf[bottom_index]);
    int code, fg, bg;
    bp_render_pair(top_color, bottom_color, &code, &fg, &bg);
    user_put(bp->x + cell_x, bp->y + cell_y,
             CELL_EXT(CELL_BLOCK, code, fg, bg));
    BP_CLEAR_DIRTY(bp->buf[top_index]);
    BP_CLEAR_DIRTY(bp->buf[bottom_index]);
}

void bp_flush(struct bp *bp) {
    for (int cell_y = 0; cell_y < bp->height/2; cell_y++)
        for (int cell_x = 0; cell_x < bp->width; cell_x++) {
            int top_index    = (cell_y * 2) * bp->width + cell_x;
            int bottom_index = top_index + bp->width;
            if (!BP_IS_DIRTY(bp->buf[top_index]) &&
                !BP_IS_DIRTY(bp->buf[bottom_index])) continue;
            uint8_t top_color    = BP_GET_COLOR(bp->buf[top_index]);
            uint8_t bottom_color = BP_GET_COLOR(bp->buf[bottom_index]);
            int code, fg, bg;
            bp_render_pair(top_color, bottom_color, &code, &fg, &bg);
            user_put(bp->x + cell_x, bp->y + cell_y,
                     CELL_EXT(CELL_BLOCK, code, fg, bg));
            BP_CLEAR_DIRTY(bp->buf[top_index]);
            BP_CLEAR_DIRTY(bp->buf[bottom_index]);
        }
}
