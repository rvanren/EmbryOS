#include "embryos.h"
#include "fdt_embryos.h"

// This is type of unallocated frames.  They point to the next unallocated frame.
union free_frame {
    union free_frame *next;
    char bytes[PAGE_SIZE];
};

extern union free_frame frames[];
static union free_frame *free_list;

void *frame_alloc(void) {
    if (free_list == 0) die("out of frames");
    union free_frame *f = free_list;
    free_list = f->next;
    L1(L_NORM, L_FRAME_ALLOC, (uintptr_t) f);
    return f;
}

void frame_release(void *frame) {
    L1(L_FREQ, L_FRAME_RELEASE, (uintptr_t) frame);
    union free_frame *f = frame;
    f->next = free_list;
    free_list = f;
}

void frame_init(void *fdt, uintptr_t end) {
    extern char _start;
    kprintf("pmemsave 0x%p 0x%p mem.bin\n", &_start, (char *) &frames - &_start);

    unsigned int total = 0;

    end &= ~(PAGE_SIZE - 1);
    if (fdt == 0) {
        for (union free_frame *f = (union free_frame *) end; --f >= frames;) {
            frame_release(f);
            total++;
        }
    }
    else {
        uintptr_t fdt_start = (uintptr_t) fdt & ~(PAGE_SIZE - 1);
        uintptr_t fdt_end = ((uintptr_t) fdt + fdt_totalsize(fdt) + PAGE_SIZE - 1)
                            & ~(PAGE_SIZE - 1);

        for (union free_frame *f = (union free_frame *) end; --f >= frames;) {
            uintptr_t addr = (uintptr_t) f;
            if (addr < fdt_start || addr > fdt_end) { // skip the FDT region
                frame_release(f);
                total++;
            }
        }
    }
    L1(L_BASE, L_FRAME_INIT, total);
    kprintf("%u frames available\n", total);
}
