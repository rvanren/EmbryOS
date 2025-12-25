#pragma once

// State of a flat file system implementation
struct flat { struct bd *lower; int stat_inode; };

// Allocate new file, return file #
int  flat_create(struct flat *fs);

// Read from a particular file # at a particular offset
int  flat_read(struct flat *fs, int file, int off, void *dst, int n);

// Write to a particular file # at a particular offset
int  flat_write(struct flat *fs, int file, int off, const void *src, int n);

// Obtain the size of a file
int  flat_size(struct flat *fs, int file);

// Delete a file
void flat_delete(struct flat *fs, int file);

// Initialize a flat file system implementation layered over block
// device 'lower'.  'format' is a boolean that decides whether the
// file system should be initialized on the block device, or if
// the block device already contains an initialized file system.
void flat_init(struct flat *fs, struct bd *lower, int format);
