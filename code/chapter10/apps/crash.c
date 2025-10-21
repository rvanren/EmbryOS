#include "syslib.h"

void main(void) {
    * (int *) 0x666 = 0x666;
}
