ENTRY(main)

SECTIONS
{
  . = 0;
  .text : {
    *(.text*)
    *(.rodata*)
  }

  .data : {
    *(.data*)
  }

  .bss (NOLOAD) : {
    *(.bss*)
    *(COMMON)
  }
}
