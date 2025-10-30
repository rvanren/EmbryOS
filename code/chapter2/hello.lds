OUTPUT_ARCH("riscv")
ENTRY(_start)

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

  /* Code and constants → RX segment */
  .text : {
    *(.text.start*)
    *(.text*)
    *(.rodata*)
  } >ram AT>ram :text

  /* Writable data → RW segment */
  .data : ALIGN(8) {
    *(.data*)
    *(.sdata*)
  } >ram AT>ram :data

  .bss (NOLOAD) : ALIGN(8) {
    *(.bss*)
    *(COMMON)
  } >ram :data

  . = ALIGN(16);
}
