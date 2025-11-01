ENTRY(_start)

SECTIONS
{
  /* Start user code/data at 0x1000 */
  . = 0x1000;

  .text : ALIGN(4) {
    *(.text*)
    *(.rodata*)
  }

  .data : ALIGN(4) {
    *(.data*)
    *(.sdata*)
  }

  /* Global pointer for small-data model */
  __global_pointer$ = . + 0x800;

  .bss (NOLOAD) : ALIGN(4) {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }
}
