#include "kprintf.h"
#include "io.h"

#define PRINTF kprintf
#define PUTCHAR(c) io_putchar(c)

#include "shared/printf_template.c"
