OUTPUT_ARCH("riscv")
ENTRY(_start)

/* --- configurable system parameters --- */
STACK_SIZE       = 4096;
PAGE_SIZE        = 8192;
FRAME_PAGES      = 32;

/* Split program headers so .text/.rodata are RX and .data/.bss are RW */
PHDRS {
  text PT_LOAD FLAGS(5);   /* PF_R | PF_X */
  data PT_LOAD FLAGS(6);   /* PF_R | PF_W */
}

MEMORY {
  ram (rwx) : ORIGIN = 0x80000000, LENGTH = 0x200000  /* 2 MiB example */
}

SECTIONS {
  . = ORIGIN(ram);

  /* --- Code and read-only data --- */
  .text : {
    *(.text.start*)
    *(.text.enter)
    *(.text .text.*)
    *(.rodata .rodata.*)
  } >ram AT>ram :text     /* note: :text AFTER >ram */

  /* --- Writable data --- */
  .data : ALIGN(8) {
    *(.data .data.*)
    *(.sdata .sdata.*)
  } >ram AT>ram :data

  /* --- Zero-initialized --- */
  .bss (NOLOAD) : ALIGN(8) {
    *(.bss .bss.*)
    *(COMMON)
  } >ram :data

  /* --- Reserved kernel areas --- */
  .frames (NOLOAD) : ALIGN(PAGE_SIZE) {
    PROVIDE(frames = .);
    . = . + (PAGE_SIZE * FRAME_PAGES);
    PROVIDE(__frames_end = .);
  } >ram :data

  .stack (NOLOAD) : ALIGN(16) {
    PROVIDE(stack_start = .);
    . = . + STACK_SIZE;
    PROVIDE(stack_end = .);
  } >ram :data

  PROVIDE(__stack_top = ORIGIN(ram) + LENGTH(ram));
}
