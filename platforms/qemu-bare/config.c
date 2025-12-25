#include "embryos.h"

struct config config = {
    // sifive_u
    // .uart_type = "uart0",
    // .uart_base = 0x10010000UL,

    // virt
    .uart_type = "ns16550a",
    .uart_base = 0x10000000UL,

    .mem_end   = 0x80100000
};
