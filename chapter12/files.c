#include "embryos.h"
#include "dir.h"

extern struct block ramdisk[], __ramdisk_end[];

struct bd ramdisk_iface;
struct ramdisk_state ramdisk_state;
struct bd simple_iface;
struct simple_state simple_state;
struct flat flat_fs;

void files_init(void) {
    bd_init();  // initialize block device module

    // Initialize the RAM disk block device layer
    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk, __ramdisk_end - ramdisk);

    // Add the "simple" block device layer
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);

    // Add and initialize the "flat file system"
    flat_init(&flat_fs, &simple_iface, 1);
    if (flat_create(&flat_fs) != ROOT_DIR) die("files_init: root dir must be 1");

    // Add the applications to the file system
    for (int i = 0; embedded_files[i].name != 0; i++) {
        L1(L_NORM, L_ADD_FILE, i);
        const char *name = embedded_files[i].name;
        for (const char *p = name; *p != 0; p++)
            if (*p == '/') name = p + 1;
        struct dirent de;
        memset(&de, 0, sizeof(de));
        strncpy(de.name, name, NAME_LEN - 1);
        de.file = flat_create(&flat_fs);
        if (de.file != i + 2) die("files_init: unexpected file number");
        flat_write(&flat_fs, de.file, 0, embedded_files[i].data,
                             embedded_files[i].size);
        flat_write(&flat_fs, ROOT_DIR, i * sizeof(de), &de, sizeof(de));
    }
}
