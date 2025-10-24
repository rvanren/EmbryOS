#pragma once

#include <stdint.h>

#define SYS_EXIT    0     // exit process
#define SYS_SPAWN   1     // start a new process
#define SYS_GET     2     // read a character from the keyboard
#define SYS_PUT     3     // write a character to the screen
#define SYS_CREATE  4     // create a new file
#define SYS_READ    5     // read a file
#define SYS_WRITE   6     // write a file
#define SYS_SIZE    7     // get the size of a file
#define SYS_DELETE  8     // delete a file

// The remaining stuff is for output

enum ansi_color { ANSI_BLACK, ANSI_RED, ANSI_GREEN, ANSI_YELLOW,
                     ANSI_BLUE,ANSI_MAGENTA, ANSI_CYAN, ANSI_WHITE };

typedef uint32_t cell_t;

#define CELL_FOREGROUND      8  // foreground color
#define CELL_BACKGROUND     16  // background color
#define CELL_ATTRIBUTES     24  // not yet supported (blinking, bold, ...)

#define CELL(ch, fg, bg)     ((uint32_t) (((ch) & 0xFF) | \
                    (((fg) & 0xFF) << CELL_FOREGROUND) | \
                    (((bg) & 0xFF) << CELL_BACKGROUND)))

#define CELL_CH(c)          (c & 0xFF)
#define CELL_FG(c)          (((c) >> CELL_FOREGROUND) & 0xFF)
#define CELL_BG(c)          (((c) >> CELL_BACKGROUND) & 0xFF)
