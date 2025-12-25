#include "embryos.h"

// Do not put log_header in .bss so that on a restart it is not cleared
struct log_header log_header = {
    .ts = { 0, 0, 0 },
    .sizes = { 1, 1, 1 }
};

void log_init(void) {
    if (log_header.ts[0] != 0) {
        kprintf("\nRestart blocked\n");
        for (;;) ;
    }
    size_t n = __log_end - log;
    log_header.sizes[0] = n / 4;
    if (log_header.sizes[0] > 100) log_header.sizes[0] = 100;
    log_header.sizes[2] = n / 4;
    log_header.sizes[1] = n - log_header.sizes[0] - log_header.sizes[2];
    L0(L_BASE, L_BOOT);
}
