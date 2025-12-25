#include "syslib.h"
#include "string.h"
#include "dir.h"

int dir_lookup(const char *name) {
    if (strcmp(name, "splash") == 0) return 3;
    if (strcmp(name, "life")   == 0) return 4;
    if (strcmp(name, "snake")  == 0) return 5;
    if (strcmp(name, "pong")   == 0) return 6;
    return -1;
}
