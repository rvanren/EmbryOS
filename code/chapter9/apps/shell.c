#include "syslib.h"

void main(void) {
    user_put(10, 3, '$', 2, 0);
    for (int counter = 0;; counter++) {
        char c = user_get();
        user_put(10, 5 + counter % 10, c, 2, 0);
        if (c == '1') user_spawn(1, 40,  0, 40, 12);
        if (c == '2') user_spawn(1, 40, 12, 40, 12);
        if (c == '3') user_spawn(1,  0, 12, 40, 12);
        if (c == '.') user_exit();
    }
}
