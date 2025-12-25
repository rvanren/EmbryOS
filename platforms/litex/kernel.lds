OUTPUT_ARCH("riscv")
ENTRY(_start)

/* --- configurable system parameters --- */
BLOCK_SIZE       = 1024;
RAMDISK_BLOCKS   =  256;
LOG_EVENT_SIZE   =   64;
N_LOG_EVENTS     =  256;

MEMORY {
  ram (rwx) : ORIGIN = 0x80000000, LENGTH = 16M
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

  .data : ALIGN(16) {
    *(.data*)

    /* RISC-V global pointer */
    . = ALIGN(16);
    PROVIDE(__global_pointer$ = . + 0x800);

    *(.sdata*)
  } >ram AT>ram :data

  .bss (NOLOAD) : ALIGN(16) {
    __bss_start = .;

    *(.bss*)
    *(.sbss*)
    *(.tbss*)
    *(.scommon)
    *(COMMON)
    *(.gnu.linkonce.b.*)

    __bss_end = .;
  } >ram :data

  .log (NOLOAD) : ALIGN(LOG_EVENT_SIZE) {
    PROVIDE(log = .);
    . = . + (LOG_EVENT_SIZE * N_LOG_EVENTS);
    PROVIDE(__log_end = .);
  } >ram :data

  .ramdisk (NOLOAD) : ALIGN(BLOCK_SIZE) {
    PROVIDE(ramdisk = .);
    . = . + (BLOCK_SIZE * RAMDISK_BLOCKS);
    PROVIDE(__ramdisk_end = .);
  } >ram :data

  /* The heap */
  .frames (NOLOAD) : ALIGN(4096) {
    PROVIDE(frames = .);
  } >ram :data

  . = ALIGN(16);
}
