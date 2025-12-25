#include "embryos.h"

static inline void user_delay(int ms) {
    L1(L_NORM, L_USER_DELAY, ms);
    while (--ms >= 0)
        for (volatile int i = 0; i < DELAY_MS; i++) ;
}

void embryos_main(void) {
    log_init();
    if (config.uart_type != 0)
        uart_init(config.uart_type, 0, config.uart_base);
    extern char _start, frames;
    kprintf("pmemsave 0x%p 0x%p mem.bin\n", &_start, &frames - &_start);
    for (;;) {
        kprintf("EmbryOS: Life Has Begun\n");
        user_delay(1000);
    }
}
