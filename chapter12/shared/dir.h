#pragma once

#include <stdint.h>

#define ROOT_DIR 1   // The one and only directory is stored in file 1

// A directory entry contains a name of up to NAME_LEN bytes and a file number
#define NAME_LEN 14
struct dirent { char name[NAME_LEN]; uint16_t file; };

// Create a new entry in the directory, mapping name to file
int dir_create(const char *name, int file);

// Delete an entry in the directory
void dir_delete(const char *name);

// List the directory
void dir_list(void (*fn)(const char *name, int file));

// Lookup the file number for the given name.  -1 on error
int dir_lookup(const char *name);
