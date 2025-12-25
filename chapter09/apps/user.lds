ENTRY(_start)

PHDRS {
  text PT_LOAD FLAGS(5);   /* R | X */
  data PT_LOAD FLAGS(6);   /* R | W */
}

SECTIONS {
  . = 0x10000;

  .text : ALIGN(4) {
    *(.text*)
    *(.rodata*)
  } :text

  .data : ALIGN(4) {
    *(.data*)
    *(.sdata*)
  } :data

  __global_pointer$ = . + 0x800;

  .bss (NOLOAD) : ALIGN(16) {
    __bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(.tbss*)
    *(.scommon)
    *(COMMON)
    __bss_end = .;
  } :data
}
