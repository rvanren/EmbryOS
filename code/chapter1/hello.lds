OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY {
  ram (rwx) : ORIGIN = 0x80200000, LENGTH = 16M
}

SECTIONS {
  . = ORIGIN(ram);

  .text : {
    *(.text.start*)
    *(.text*)
    *(.rodata*)
  } > ram

  .data : {
    *(.data*)
    *(.sdata*)
  } > ram

  .bss (NOLOAD) : ALIGN(8) {
    *(.bss*)
    *(COMMON)
  } > ram

  . = ALIGN(16);
}
