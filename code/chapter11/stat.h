#pragma once

#include <stdint.h>

struct stat_entry {
    uint32_t inode;     // inode number of the file's data blocks
    uint32_t size;      // file size in bytes
};
