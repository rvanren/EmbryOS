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

  /* Define global pointer 0x800 bytes past start of sdata region */
  __global_pointer$ = . + 0x800;

  .bss (NOLOAD) : {
    *(.bss*)
    *(COMMON)
  }
}
