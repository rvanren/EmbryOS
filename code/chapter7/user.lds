OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
    . = 0x80400000;      /* where user space begins */

    .text : {
        *(.text*)
    }
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss : {
        *(.bss*) *(COMMON)
    }
}
