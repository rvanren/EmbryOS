#include <stdint.h>
#include "app_info.h"
#include "bd_ramdisk.h"
#include "bd_simple.h"

extern struct block ramdisk[], __ramdisk_end[];

struct bd ramdisk_iface;
struct ramdisk_state ramdisk_state;
struct bd simple_iface;
struct simple_state simple_state;

void files_init(void) {
    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk, __ramdisk_end - ramdisk);
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);
}
