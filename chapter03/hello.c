#include "embryos.h"
#include "fdt_embryos.h"

struct hart hart;

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

    // Create the initial process
    hart.id  = hartid;
    struct pcb *self = proc_create(&hart, -1, (struct rect){ 0, 0, 80, 24 }, 0, 0);
    sched_set_self(self);
    L2(L_BASE, L_HART_INIT, hartid, 0);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));
    extern char _start, frames;
    kprintf("\n\r\033[30m\033[47mpmemsave 0x%p 0x%p mem.bin", &_start, &frames - &_start);

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
