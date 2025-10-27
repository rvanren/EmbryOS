#include <stdarg.h>
#include "kprintf.h"
#include "uart.h"

#define PRINTF kprintf
#define PUTCHAR(c) uart_putchar(c)

#include "shared/printf_template.c"
