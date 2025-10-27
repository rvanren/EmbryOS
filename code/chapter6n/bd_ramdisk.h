#pragma once

#include "bd.h"

struct ramdisk_state {
    int nblocks;
    char *data;
};

void ramdisk_init(struct bd *iface, struct ramdisk_state *state,
                    void *mem, int nblocks);
