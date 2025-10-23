#include <stddef.h>
#include "syslib.h"
#include "string.h"

#define MAX_ARGS    16

void _crt_init(const char *argbuf, size_t arglen) {
    extern int main(int argc, char **argv);
    int argc = 0;
    char *argv[MAX_ARGS];
    const char *p = argbuf;
    const char *end = argbuf + arglen;

    while (p < end && *p != '\0' && argc < (int)(sizeof(argv)/sizeof(argv[0])) - 1) {
        argv[argc++] = (char *) p;
        p += strlen(p) + 1;
    }
    argv[argc] = NULL;

    main(argc, argv);
    user_exit();
}
