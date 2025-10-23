#include <stdint.h>
#include "app_info.h"
#include "bd_ramdisk.h"
#include "bd_simple.h"
#include "flat.h"

#define MAX_APPS    32

extern struct block ramdisk[], __ramdisk_end[];

struct bd ramdisk_iface;
struct ramdisk_state ramdisk_state;
struct bd simple_iface;
struct simple_state simple_state;
struct flat flat_fs;

static void write_app(struct flat *ffs, int file_id, uint32_t gp,
                      const char *start, const char *end) {
    uint32_t size = (uint32_t)(end - start);
    flat_write(ffs, file_id, 0, &gp, 4);
    flat_write(ffs, file_id, 4, start, size);
}

void files_init(void) {
    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk, __ramdisk_end - ramdisk);
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);
    flat_init(&flat_fs, &simple_iface, 1);

    (void) flat_create(&flat_fs);       // empty root directory
    for (int i = 0; i < n_applications; i++) {
        int f = flat_create(&flat_fs);
        write_app(&flat_fs, f, app_table[i].gp,
                  app_table[i].start, app_table[i].end);
    }
}
