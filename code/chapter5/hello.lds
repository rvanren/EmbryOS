OUTPUT_ARCH("riscv")
ENTRY(_start)

/* --- configurable system parameters --- */
STACK_SIZE       = 4096;
PAGE_SIZE        = 8192;
FRAME_PAGES      = 32;
BLOCK_SIZE       = 512;
RAMDISK_BLOCKS   = 2048;

MEMORY
{
    ram (arw!xi) : ORIGIN = 0x80000000, LENGTH = 0x200000  /* 2 MiB for example */
}

SECTIONS
{
    . = ORIGIN(ram);

    .text : {
        *(.text.enter)
        *(.text .text.*)
        *(.rodata .rodata.*)
    } >ram

    .data : ALIGN(8) {
        *(.data .data.*)
        *(.sdata .sdata.*)
    } >ram

    .bss (NOLOAD) : ALIGN(8) {
        *(.bss .bss.*)
        *(COMMON)
    } >ram

    /* Reserve space for ramdisk */
    .ramdisk (NOLOAD) : ALIGN(BLOCK_SIZE) {
        PROVIDE(ramdisk = .);
        . = . + (BLOCK_SIZE * RAMDISK_BLOCKS);
        PROVIDE(__ramdisk_end = .);
    } >ram

    /* Reserve frames for page allocator */
    .frames (NOLOAD) : ALIGN(PAGE_SIZE) {
        PROVIDE(frames = .);
        . = . + (PAGE_SIZE * FRAME_PAGES);
        PROVIDE(__frames_end = .);
    } >ram

    /* Kernel stack */
    .stack (NOLOAD) : ALIGN(16) {
        PROVIDE(stack_start = .);
        . = . + STACK_SIZE;
        PROVIDE(stack_end = .);
    } >ram

    /* Symbol marking top of usable RAM */
    PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));  /* top of RAM */
}
