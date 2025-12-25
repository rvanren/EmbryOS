#include "embryos.h"

struct config config = {
    .uart_type = "litex",
    .uart_base = 0xF0001000UL,
    .mem_end   = 0x80100000
};
