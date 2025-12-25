#pragma once

#include <stdint.h>

// Lookup the file number for the given name.  -1 on error
int dir_lookup(const char *name);
