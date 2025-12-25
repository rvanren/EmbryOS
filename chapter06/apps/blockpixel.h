#pragma once

// The "block pixel" module manages a grid of ANSI colors.  Each pixel
// occupies have of a character: either the top half or the bottom half.
// Unfortunately, that's about the best we can do as quadrant characters
// are not complete and sextant characters are not portable.

#include <stdint.h>

enum bp_mode { BP_EAGER, BP_LAZY };

struct bp {
    int x, y;             // screen position of top-left corner
    int width, height;    // height must be even
    uint8_t *buf;         // width * height bytes
};

// Initialize a grid starting at upper left position (x, y).
// Requirements:
//      - the buffer must be of size width * height
//      - y and height must be even
void bp_init(struct bp *bp,
             int x, int y,
             int width, int height,
             uint8_t *buffer);

// Update the color of (px, py).  mode is either BP_EAGER (do it now)
// or BP_LAZY (do it when bp_flush() is invoked).  Use BP_LAZY if you
// want to update a bunch of pixels at the same time.
void bp_put(struct bp *bp,
            int px, int py,
            uint8_t color,
            enum bp_mode mode);

// Make all the 'lazy' updates.
void bp_flush(struct bp *bp);
