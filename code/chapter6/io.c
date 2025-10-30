#include "sbi.h"
#include "io.h"

void io_putchar(char c) {
    sbi_putchar(c);
}
