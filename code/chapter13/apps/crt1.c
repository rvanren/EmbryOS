#include "syslib.h"

void _crt_init(char *arg_buf, int size) {
    extern void main(int argc, char **argv);
    main(0, 0);
    user_exit();
}
