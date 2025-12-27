ENTRY(_start)

PHDRS {
  text PT_LOAD FLAGS(5);   /* R | X */
  data PT_LOAD FLAGS(6);   /* R | W */
}

SECTIONS {
  /* File offset for the whole flat image */
  flat_file_off = 0x1000;

  /* Virtual address of program image */
  flat_vaddr = 0x70000000;
  . = flat_vaddr;

  /* TEXT AND RODATA (RX) */
  _image_start = .;

  .text ALIGN(4) : AT(flat_file_off) {
      *(.text*)
      *(.rodata*)
  } :text

  /* current file offset after text */
  text_file_end = flat_file_off + SIZEOF(.text);

  /* DATA and BSS (RW) */
  .data ALIGN(4) : AT(text_file_end) {
      *(.data*)
      *(.sdata*)

      . = ALIGN(16);
      __bss_start = .;
      *(.bss*)
      *(.sbss*)
      *(.tbss*)
      *(.scommon)
      *(COMMON)
      __bss_end = .;
  } :data

  _image_end = .;

  /* GP (RISC-V ABI) */
  __global_pointer$ = flat_vaddr + 0x800;
}
