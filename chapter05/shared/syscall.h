#pragma once

#include <stdint.h>

#define USER_GET_NO_INPUT    0
#define USER_GET_GOT_FOCUS   (-1)
#define USER_GET_LOST_FOCUS  (-2)

enum ansi_color { ANSI_BLACK, ANSI_RED, ANSI_GREEN, ANSI_YELLOW,
                     ANSI_BLUE,ANSI_MAGENTA, ANSI_CYAN, ANSI_WHITE };
enum cell_type { CELL_ASCII, CELL_BLOCK, CELL_BRAILLE };

typedef uint16_t cell_t;

#define CELL_FOREGROUND      8  // foreground color shift
#define CELL_BACKGROUND     11  // background color shift
#define CELL_TYPE           14  // ASCII, BLOCK, BRAILLE

#define CELL_EXT(type, c, fg, bg)     ((cell_t) (((c) & 0xFF) | \
                    (((fg) & 0x7) << CELL_FOREGROUND) | \
                    (((bg) & 0x7) << CELL_BACKGROUND) | \
                    (((type) & 0x3) << CELL_TYPE)))

#define CELL(ch, fg, bg)     CELL_EXT(CELL_ASCII, (ch), (fg), (bg))

#define CELL_CH(c)          (c & 0xFF)
#define CELL_FG(c)          (((c) >> CELL_FOREGROUND) & 0x7)
#define CELL_BG(c)          (((c) >> CELL_BACKGROUND) & 0x7)
#define CELL_T(c)           (((c) >> CELL_TYPE) & 0x3)
