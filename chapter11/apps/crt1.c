#include <stdint.h>
#include "syslib.h"

void _crt_init(char **argv) {
    extern int main(int argc, char **argv);
    (void) main((uintptr_t) argv[0], argv + 1);
    user_exit();
}
