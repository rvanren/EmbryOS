OUTPUT_ARCH("riscv")                                                            ENTRY(_start)                                                                                                                                                   MEMORY                                                                          {                                                                                   ram (arw!xi) : ORIGIN = 0x80000000, LENGTH = 0x200000                       }

PHDRS
{
    text PT_LOAD FLAGS(5);   /* R + X = 0x5 */
    data PT_LOAD FLAGS(6);   /* R + W = 0x6 */
}

SECTIONS
{
    .init : ALIGN(8) {
        *(.text.enter)
    } >ram :text

    .text : ALIGN(8) {
        *(.text .text.*)
    } >ram :text

    .rodata : ALIGN(8) {
        *(.rdata)
        *(.rodata .rodata.*)
        . = ALIGN(8);
        *(.srodata .srodata.*)
    } >ram :text

    .data : ALIGN(8) {
        *(.data .data.*)
        . = ALIGN(8);
        *(.sdata .sdata.* .sdata2.*)
    } >ram :data

    .bss (NOLOAD) : ALIGN(8) {
        *(.sbss*)
        *(.bss .bss.*)
        *(COMMON)
    } >ram :data

    .frames (NOLOAD) : ALIGN(4096) {
        PROVIDE(frames = .);
        . = . + (4096 * 64);    /* 64 pages (256 KiB) */
        PROVIDE(__frames_end = .);
    } >ram :data
}
