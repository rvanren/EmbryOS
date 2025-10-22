#include <stdint.h>
#include "apps_gen.h"
#include "bd_ramdisk.h"
#include "bd_simple.h"
#include "flat.h"

#define MAX_APPS    256

#define RAMDISK_SIZE 1024
struct block ramdisk_blocks[BLOCK_SIZE];

int applications[MAX_APPS];

static void write_app(int file_id, uint32_t gp,
                      const char *start, const char *end) {
    uint32_t size = (uint32_t)(end - start);
    flat_write(&flat_fs, file_id, 0, &gp, 4);
    flat_write(&flat_fs, file_id, 4, start, size);
}

void files_init(void) {
    struct bd ramdisk_iface;
    struct ramdisk_state ramdisk_state;
    struct bd simple_iface;
    struct inode_layer_state simple_state;
    struct flat flat_fs;

    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk_blocks, RAMDISK_SIZE);
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);
    flat_init(&flat_fs, &simple_iface, 1);

    for (int i = 0; i < n_applications; i++) {
        int f = flat_create(&flat_fs);
        applications[i] = f;
        write_app(f, app_table[i].gp,
                  app_table[i].start, app_table[i].end);
    }
}
