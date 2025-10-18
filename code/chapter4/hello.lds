OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    ram (arw!xi) : ORIGIN = 0x80000000, LENGTH = 0x200000
}

PHDRS
{
    ram PT_LOAD;
}

SECTIONS
{
    .init : ALIGN(8) {
        *(.text.enter)
    } >ram :ram

    .text : ALIGN(8) {
        *(.text .text.*)
    } >ram :ram

    .rodata : ALIGN(8) {
        *(.rdata)
        *(.rodata .rodata.*)
        . = ALIGN(8);
        *(.srodata .srodata.*)
    } >ram :ram

    .data : ALIGN(8) {
        *(.data .data.*)
        . = ALIGN(8);
        *(.sdata .sdata.* .sdata2.*)
    } >ram :ram

    .bss (NOLOAD) : ALIGN(8) {
        *(.sbss*)
        *(.bss .bss.*)
        *(COMMON)
    } >ram :ram

    /* ---------------------------------------------------------
     * Page-aligned frame allocator region (our heap)
     * --------------------------------------------------------- */
    .frames (NOLOAD) : ALIGN(4096) {
        PROVIDE(frames = .);
        . = . + (4096 * 64);    /* 64 pages (256 KiB) */
        PROVIDE(__frames_end = .);
    } >ram :ram
}
