#include <stddef.h>
#include "syslib.h"
#include "string.h"

#define MAX_ARGS    16

void _crt_init(const char *argbuf, size_t arglen) {
    extern int main(int argc, char **argv);
    main(0, 0);
    user_exit();
}
