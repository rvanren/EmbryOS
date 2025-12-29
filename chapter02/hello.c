#include "embryos.h"

void print_string(int col, int row, char *str) {
    while (*str != 0) {
        screen_put(col, row, CELL(*str, ANSI_YELLOW, ANSI_RED));
        str++;
        col++;
    }
}

int embryos_main(void) {
    log_init();
    if (config.uart_type != 0)
        uart_init(config.uart_type, 0, config.uart_base);
    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));
    extern char _start, frames;
    kprintf("\n\r\033[30m\033[47mpmemsave 0x%p 0x%p mem.bin",
                            &_start, &frames - &_start);

    print_string(50, 5, "EmbryOS");
    print_string(47, 7, "Life has Begun");

    extern void blockdemo_main();
    blockdemo_main();
    for (;;) ;
}
