#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "platform.h"
#include "types.h"
#include "stdlib.h"
#include "malloc.h"

#define ALMSK       ((1 << 4) - 1)    // 16 bytes alignment 
#define ROUNDUP(x)  (((uword_t)(x) + ALMSK) & ~ (uword_t)ALMSK)

struct m_hdr {
    struct m_hdr *next;
    size_t size;
};

static struct m_hdr *free_list, *brk;
extern struct m_hdr __bss_end[];

void *malloc(size_t size) {
    struct m_hdr *mh;
    size = ROUNDUP(sizeof(*mh) + size);
    for (struct m_hdr **pmh = &free_list; (mh = *pmh); pmh = &mh->next)
        if (mh->size == size) {
            *pmh = mh->next;
            return &mh[1];
        }
    if (!brk) brk = (struct m_hdr *) ROUNDUP(&__bss_end[1]) - 1;
    char *next = (char *) brk + size;
    if (next > (char *) VM_END) return 0;
    mh = brk;
    mh->size = size;
    brk = (struct m_hdr *) next;
    return &mh[1];
}

void *calloc(size_t nitems, size_t size) {
    size *= nitems;
    void *p = malloc(size);
    if (p != 0) memset(p, 0, size);
    return p;
}

void *realloc(void *ptr, size_t size) {
    if (ptr == 0) return malloc(size);
    struct m_hdr *mh = (struct m_hdr *) ptr - 1;
    size = ROUNDUP(sizeof(*mh) + size);
    if (size <= mh->size) return ptr;
    void *newp = malloc(size - sizeof(*mh));
    if (newp == 0) return 0;
    memcpy(newp, ptr, mh->size - sizeof(*mh));
    free(ptr);
    return newp;
}

void free(void *ptr) {
    if (ptr == 0) return;
    struct m_hdr *mh = (struct m_hdr *) ptr - 1;
    mh->next = free_list;
    free_list = mh;
}
