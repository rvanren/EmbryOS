#pragma once

#include <stdint.h>

struct app_info {
    const char *start, *end;
    uint32_t gp;
};

extern struct app_info app_table[];
extern int n_applications;
