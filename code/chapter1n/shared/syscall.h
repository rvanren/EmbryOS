#pragma once

#include <stdint.h>
#include "platform.h"

enum syscall {
    SYS_EXIT,       // exit process
    SYS_SPAWN,      // start a new process
    SYS_GET,        // read a character from the keyboard
    SYS_PUT,        // write a character to the screen
    SYS_CREATE,     // create a new file
    SYS_READ,       // read a file
    SYS_WRITE,      // write a file
    SYS_SIZE,       // get the size of a file
    SYS_DELETE      // delete a file
};

// The remaining stuff is for output

enum ansi_color { ANSI_BLACK, ANSI_RED, ANSI_GREEN, ANSI_YELLOW,
                     ANSI_BLUE,ANSI_MAGENTA, ANSI_CYAN, ANSI_WHITE };

typedef uint16_t cell_t;

#define CELL_FOREGROUND      8  // foreground color shift
#define CELL_BACKGROUND     11  // background color shift
#define CELL_ATTRIBUTES     14  // 2 bits for attrs (blinking, bold?)

#define CELL(ch, fg, bg)     ((cell_t) (((ch) & 0xFF) | \
                    (((fg) & 0x7) << CELL_FOREGROUND) | \
                    (((bg) & 0x7) << CELL_BACKGROUND)))

#define CELL_CH(c)          (c & 0xFF)
#define CELL_FG(c)          (((c) >> CELL_FOREGROUND) & 0x7)
#define CELL_BG(c)          (((c) >> CELL_BACKGROUND) & 0x7)

static inline user_delay(int ms) {  // pseudo system call
    while (ms > 0) {
        for (volatile int i = 0; i < DELAY_MS; i++) ;
        ms--;
    }
}
