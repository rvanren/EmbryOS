#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "uart.h"

int main(void) {
    uart_init();
    for (;;) {
        kprintf("EmbryOS: Life Has Begun\n");
        user_delay(1000);
    }
}
