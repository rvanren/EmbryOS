#pragma once

#include <stdint.h>

struct app_info {
    const char *name;
    uintptr_t base, end, gp_offset;
};
