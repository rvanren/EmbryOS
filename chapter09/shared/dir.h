#pragma once

#define ROOT_DIR 1   // The one and only directory is stored in file 1

// Lookup the file number for the given name.  -1 on error
int dir_lookup(const char *name);
