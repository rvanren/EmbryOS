#include "kprintf.h"
#include "syscall.h"

int main(void) {
    for (;;) {
        kprintf("EmbryOS: Life Has Begun\n");
        user_delay(1000);
    }
}
