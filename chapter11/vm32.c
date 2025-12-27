#include "embryos.h"

enum { V, R, W, X, U, G, A, D };
#define PTE(x)              ((uword_t) 1 << (x))
#define PT_ENTRY(pa, flags) ((((uword_t)(pa) & ~ (uword_t) 0xFFF) >> 2) | (flags))
#define PTE_COUNT           (PAGE_SIZE / sizeof(uword_t))
#define BPW                 (8 * sizeof(uword_t))   // bits per word
#define RW                  (PTE(V)|PTE(R)|PTE(W)|PTE(A)|PTE(D))
#define RWX                 (RW|PTE(X))

#if VBITS == 32

void vm_map(void *base, uintptr_t va, void *frame) {
    uword_t *pt = base;
    int index = (va >> 12) & (PTE_COUNT - 1);
    pt[index] = PT_ENTRY((uintptr_t) frame, RWX|PTE(U));
}

int vm_is_mapped(void *base, uintptr_t va) {
    uword_t *pt = base;
    int index = (va >> 12) & (PTE_COUNT - 1);
    return pt[index] & PTE(V);
}

void vm_flush(struct hart *hart, void *base) {
    const int index = (VM_START >> 22) & (PTE_COUNT - 1);  // 12 + 10
    hart->parent_page_table[index] = PT_ENTRY((uintptr_t) base, PTE(V));
    tlb_flush();
}

void vm_release(void *base) {
    uword_t *pt = base;
    for (int i = 0; i < PTE_COUNT; i++) {
        uword_t pte = pt[i];
        if (pte & PTE(V)) frame_release((void *)((uintptr_t)(pte >> 10) << 12));
    }
}

void vm_init(struct hart *hart) {
    hart->parent_page_table = frame_alloc();
    for (int i = 0; i < PTE_COUNT; i++)
        hart->parent_page_table[i] = PT_ENTRY((uword_t) i << 22, RWX|PTE(G));

    if (hart->idx == 0) kprintf("Enabling virtual memory now\n");
    vm_enable(((uword_t) 1 << (BPW - 1)) |
                                (((uword_t) hart->parent_page_table) >> 12));
    tlb_flush();
    if (hart->idx == 0) kprintf("Virtual memory enabled\n");
}

#endif // VBITS == 32
