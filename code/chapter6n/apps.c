#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "pmp.h"
#include "kprintf.h"
#include "string.h"
#include "interrupt.h"

#ifdef CH11
#include "flat.h"
extern struct flat flat_fs;
#else
#ifdef CH8
#include "app_info.h"
__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val,
                uintptr_t user_sp, size_t arg_size, uintptr_t ksp);
#else
void init_main(), splash_main(), life_main(), snake_main();
#ifdef CH7
static inline void user_exit() {
    register int a7 asm("a7") = SYS_EXIT;
    asm volatile("ecall" : : "r"(a7));
}
void init_crt(){ init_main(); user_exit(); }
void splash_crt(){ splash_main(); user_exit(); }
void life_crt(){ life_main(); user_exit(); }
void snake_crt(){ snake_main(); user_exit(); }
static void (*apps[])() = { init_crt, splash_crt, life_crt, snake_crt };

__attribute__((noreturn)) void enter_user(void (*entry)());
#else
static void (*apps[])() = { init_main, splash_main, life_main, snake_main };
#endif
#endif
#endif

void exec_user(void) {
    struct pcb *self = run_queue[proc_current]->next;

#ifdef CH8
    self->base = frame_alloc();
    self->stack = frame_alloc();
    if (self->base == 0 || self->stack == 0) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("out of memory<");
        proc_exit();
    }

    uint32_t gp_offset;

#ifdef CH11
    flat_read(&flat_fs, self->executable, 0, &gp_offset, sizeof(gp_offset));
    uint32_t size = flat_size(&flat_fs, self->executable) - sizeof(gp_offset);
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("executable too large<");
        proc_exit();
    }

    // Initialize code/data page
    flat_read(&flat_fs, self->executable, sizeof(gp_offset), self->base, size);
#else
    struct app_info *ai = &app_table[self->executable - 2];
    gp_offset = ai->gp;

    uint32_t size = ai->end - ai->start;
    if (size > PAGE_SIZE) {
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
        kprintf("executable too large<");
        proc_exit();
    }

    memcpy(self->base, ai->start, size);
#endif

    memset(&self->base[size], 0, PAGE_SIZE - size);
    memset(self->stack, 0, PAGE_SIZE);

    uintptr_t sp = (uintptr_t) self->stack + PAGE_SIZE;
#ifdef CH13
    // Initialize stack page
    sp -= self->size;
    sp &= ~0xF;   // align down to 16 bytes
    memcpy((void *) sp, self->args, self->size);
#endif

#ifdef CH10
    // Load PMP registers
    pmp_load(self);
#endif

    enter_user(self->base, (uintptr_t) (self->base + gp_offset), sp, self->size,
                            (uintptr_t) self + PAGE_SIZE);
#else
#ifdef CH7
    enter_user(apps[self->executable - 2]);
#else
    intr_enable();
    apps[self->executable - 2]();
    intr_disable();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("process ended<");
    proc_exit();
#endif
#endif
}
