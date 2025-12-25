#include "syslib.h"

void _crt_init(void) {
    extern int main(int argc, char **argv);
    main(0, 0);
    user_exit();
}
