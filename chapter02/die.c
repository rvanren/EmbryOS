#include "embryos.h"

void die(void *msg) {
    L0(L_NORM, L_DIE);
    kprintf("%s", msg);
    for (;;) ;
}
