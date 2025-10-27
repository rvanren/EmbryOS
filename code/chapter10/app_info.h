#pragma once

#include <stdint.h>

// Application binaries are loaded directly into memory before written
// to files.  This structure contains the start and end address of a
// binary in memory, as well as its gp offset (for location-independence)
struct app_info {
    const char *start, *end;
    uint32_t gp;
};

// The list of applications can be found in the auto-generated file
// "apps_gen.c".
extern struct app_info app_table[];
extern int n_applications;
