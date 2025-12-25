#include "syslib.h"
#include "string.h"
#include "dir.h"

int dir_lookup(const char *name) {
    if (strcmp(name, "splash") == 0) return 3;
    if (strcmp(name, "life")   == 0) return 4;
    return -1;
}
