#include "syslib.h"
#include "dir.h"

void main(void) {
    user_spawn(dir_lookup("shell"),   0,  0, 39, 11, 0, 0);
    user_spawn(dir_lookup("splash"), 40,  0, 39, 11, 0, 0);
}
