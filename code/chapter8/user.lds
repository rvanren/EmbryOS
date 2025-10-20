ENTRY(_start)

SECTIONS
{
  . = ALIGN(4);
  .text : {
    *(.text*)
    *(.rodata*)
  }

  .data : {
    *(.data*)
    *(.sdata*)
  }

  /* Define where gp should point for small-data access */
  __global_pointer$ = . + 0x800;

  .bss (NOLOAD) : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }
}
