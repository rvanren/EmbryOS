#pragma once

#include <stdint.h>

struct embedded_file {
    const char *name;
    const unsigned char *data;
    size_t size;
};

// The list of files can be found in the auto-generated file "apps_gen.c".
extern const struct embedded_file embedded_files[];
