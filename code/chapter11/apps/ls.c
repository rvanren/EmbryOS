#include <stdarg.h>
#include "syslib.h"
#include "stdio.h"
#include "dir.h"

void list(const char *name, int file) {
    printf("%s %d\n", name, user_size(file));
}

void main(void) {
    dir_list(list);
}
