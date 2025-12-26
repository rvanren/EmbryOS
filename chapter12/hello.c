#include "embryos.h"
#include "fdt_embryos.h"

extern struct hart harts[];        // one entry for each hart
extern void hart_init(uword_t hartid, struct pcb *self);
extern void hart_start_others(void *fdt);
uint64_t time_base;                // #ticks per second

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    if (self != 0 && self->executable > 0)
        proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%D sepc=%X stval=%X",
                        tf->scause & 0xFFF, tf->sepc, tf->stval);
    if (self != 0 && self->executable > 0) {
        kprintf("<");
        sched_exit();
    }
    else {
        kprintf("\n");
        for (;;) ;
    }
}

void embryos_main(uword_t hartid, void *fdt) {
    log_init();

    // First initialize the UART
    if (config.uart_type != 0) uart_init(config.uart_type, 0, config.uart_base);
    else {
        int len; uintptr_t uart_base;
        const char *compat = fdt_uart_info(fdt, &len, &uart_base);
        uart_init(compat, len, uart_base);
    }

    kprintf("hartid    = %X\n", hartid);
    kprintf("fdt       = %p\n", fdt);
    // if (fdt != 0) fdt_dump(fdt);
    if (config.mem_end != 0) frame_init(0, config.mem_end);
    else {
        uintptr_t mem_base, mem_end;
        fdt_memory_range(fdt, &mem_base, &mem_end);
        kprintf("Memory: base=%X size=%X\n", mem_base, mem_end - mem_base);
        frame_init(fdt, mem_end);
    }

    if (fdt != 0) time_base = fdt_get_timebase(fdt);
    else time_base = config.time_base;
    L1(L_BASE, L_TIME_BASE, time_base);

    struct pcb *self = proc_create(&harts[0], -1, (struct rect){ 0, 0, 80, 24 }, 0, 0);
    hart_init(hartid, self); files_init();
    if (fdt != 0) hart_start_others(fdt);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                         CELL(' ', ANSI_WHITE, ANSI_BLACK));
    extern char _start, frames;
    kprintf("\n\r\033[30m\033[47mpmemsave 0x%p 0x%p mem.bin", &_start, &frames - &_start);

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}

void hart_main(uword_t hartid) {
    hart_init(hartid, sched_self());
    sched_idle();
}
