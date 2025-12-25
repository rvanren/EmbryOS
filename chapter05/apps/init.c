#include "syslib.h"
#include "dir.h"

void init_main(void) {
    user_spawn(dir_lookup("snake"),   0,  0, 39, 11, 0, 0);
    user_spawn(dir_lookup("pong"),    0, 12, 39, 11, 0, 0);
    user_spawn(dir_lookup("life"),   40, 12, 39, 11, 0, 0);
    user_spawn(dir_lookup("splash"), 40,  0, 39, 11, 0, 0);
}
