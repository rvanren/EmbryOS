#pragma once

#include <stdint.h>
#include "bd.h"

#define STAT_PER_BLOCK (BLOCK_SIZE / sizeof(struct stat_entry))

// One of these for each flat file.
struct stat_entry {
    uint32_t inode;     // inode number of the file's data blocks
    uint32_t size;      // file size in bytes
};
