#include <stdint.h>
#include "app_info.h"
#include "bd_ramdisk.h"
#include "bd_simple.h"
#include "flat.h"

#define MAX_APPS    32

extern struct block ramdisk[], ramdisk_end[];

struct bd ramdisk_iface;
struct ramdisk_state ramdisk_state;
struct bd simple_iface;
struct simple_state simple_state;
struct flat flat_fs;

void files_init(void) {
    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk, ramdisk_end - ramdisk);
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);
    flat_init(&flat_fs, &simple_iface, 1);

    (void) flat_create(&flat_fs);       // empty root directory

    // Add the applications to the file system
    for (int i = 0; i < n_applications; i++) {
        int f = flat_create(&flat_fs);
        flat_write(&flat_fs, f, 0, &app_table[i].gp, 4);
        flat_write(&flat_fs, f, 4, app_table[i].start,
                            app_table[i].end - app_table[i].start);
    }
}
