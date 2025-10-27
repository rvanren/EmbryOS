#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void init_main(void) {
    user_spawn(dir_lookup("splash"), 40,  0, 40, 12, 0, 0);
    user_spawn(dir_lookup("life"),    0, 12, 40, 12, 0, 0);
    user_spawn(dir_lookup("life"),   40, 12, 40, 12, 0, 0);
}
