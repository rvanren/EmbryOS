#include "kprintf.h"

int main(void) {
    for (;;) {
        kprintf("EmbryOS: Life Has Begun\n");
        user_delay(1000);
    }
}
