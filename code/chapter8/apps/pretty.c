#include "syslib.h"

static char chars[] = "!@#$%^&*+~:?";

void main(void) {
    user_put(0, 0, 'X', 2, 5);
#ifdef notdef
    for (;;) {
        for (int x = 0; x < 7; x++) {
            for (int y = 0; y < 7; y++) {
                user_put(x, y, chars[(x + y) % sizeof(chars)],
                    ((x + 1) * (y + 1)) % 8, ((x + 2) * (y + 2) % 8));
            }
        }
    }
#endif
}
