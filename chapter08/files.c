#include "embryos.h"

extern struct block ramdisk[], __ramdisk_end[];

struct bd ramdisk_iface;
struct ramdisk_state ramdisk_state;
struct bd simple_iface;
struct simple_state simple_state;

void files_init(void) {
    // Initialize the RAM disk block device layer
    ramdisk_init(&ramdisk_iface, &ramdisk_state,
                 ramdisk, __ramdisk_end - ramdisk);

    // Add the "simple" block device layer
    simple_init(&simple_iface, &simple_state,
                &ramdisk_iface, 0, 1);

    // Unit test
#define N_RUNS      10
    static struct block b;
    for (int i = 0; i < N_RUNS; i++) {
        L1(L_NORM, L_UT_SIMPLE_RUN, i);
        int ino = simple_iface.alloc(&simple_state);
        int size = simple_iface.size(&simple_state, ino);
        L3(L_NORM, L_UT_SIMPLE_START, i, ino, size);
        for (int k = 0; k < size; k++) {
            L2(L_FREQ, L_UT_SIMPLE_CHK1, i, k);
            simple_iface.read(&simple_state, ino, k, &b);
            if (b.bytes[0] || b.bytes[sizeof(b) - 1]) die("block not cleared");
            L2(L_FREQ, L_UT_SIMPLE_CHK2, i, k);
            b.bytes[0] = b.bytes[sizeof(b) - 1] = (char) k;
            simple_iface.write(&simple_state, ino, k, &b);
        }
        for (int k = 0; k < size; k++) {
            L2(L_FREQ, L_UT_SIMPLE_CHK3, i, k);
            simple_iface.read(&simple_state, ino, k, &b);
            if (b.bytes[0] != (char) k || b.bytes[sizeof(b) - 1] != (char) k)
                die("unexpected byte");
        }
        L2(L_NORM, L_UT_SIMPLE_FREE, i, ino);
        simple_iface.free(&simple_state, ino);
    }
}
