#include "embryos.h"
#include "libfdt.h"

void fdt_dump(void *fdt);
void fdt_memory_range(const void *fdt, uintptr_t *base_out, uintptr_t *end_out);
const char *fdt_uart_info(const void *fdt, int *plen, uintptr_t *pbase);
uint32_t fdt_get_timebase(void *fdt);
