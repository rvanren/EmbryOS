OUTPUT_ARCH("riscv")
ENTRY(_start)

/* --- configurable system parameters --- */
PAGE_SIZE        = 8192;
FRAME_PAGES      = 32;

MEMORY {
  ram (rwx) : ORIGIN = 0x80400000, LENGTH = 16M
}

/* ----------------------------------------------------------- */
/*  Define two ELF program headers: one RX, one RW             */
/* ----------------------------------------------------------- */
PHDRS {
  text PT_LOAD FLAGS(5);   /* PF_R | PF_X */
  data PT_LOAD FLAGS(6);   /* PF_R | PF_W */
}

SECTIONS {
  . = ORIGIN(ram);

  /* Code and constants - RX segment */
  .text : {
    *(.text.start*)
    *(.text*)
    *(.rodata*)
  } >ram AT>ram :text

  /* Writable data - RW segment */
  .data : ALIGN(16) {
    *(.data*)
    *(.sdata*)
  } >ram AT>ram :data

  .bss (NOLOAD) : ALIGN(16) {
    *(.bss*)
    *(COMMON)
  } >ram :data

  /* The heap */
  .frames (NOLOAD) : ALIGN(PAGE_SIZE) {
    PROVIDE(frames = .);
    . = . + (PAGE_SIZE * FRAME_PAGES);
    PROVIDE(__frames_end = .);
  } >ram :data

  . = ALIGN(16);
}
