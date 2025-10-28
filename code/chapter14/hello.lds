OUTPUT_ARCH("riscv")
ENTRY(_start)

/* --- configurable system parameters --- */
MAX_CPU          = 4;         /* maximum number of harts */
STACK_SIZE       = 4096;      /* per-hart stack size */
PAGE_SIZE        = 8192;      /* page size */
FRAME_PAGES      = 32;        /* number of 4 KiB frames reserved */
BLOCK_SIZE       = 512;       /* size of a diskblock
RAMDISK_BLOCKS   = 32;        /* number of 4 KiB pages for ramdisk */

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
        PROVIDE(ramdisk_end = .);
    } >ram

    /* Reserve frames for page allocator */
    .frames (NOLOAD) : ALIGN(PAGE_SIZE) {
        PROVIDE(frames = .);
        . = . + (PAGE_SIZE * FRAME_PAGES);
        PROVIDE(frames_end = .);
    } >ram

    /* Per-hart kernel stacks */
    .stacks (NOLOAD) : ALIGN(16) {
        PROVIDE(stacks_start = .);
        . = . + (STACK_SIZE * MAX_CPU);
        PROVIDE(stacks_end = .);
    } >ram

    /* Symbol marking top of usable RAM */
    PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));  /* top of RAM */
    PROVIDE(stacks_start);
    PROVIDE(stacks_end);
    PROVIDE(STACK_SIZE);
}
