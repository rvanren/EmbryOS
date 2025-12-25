# Chapter 01: Hello World

This chapter brings a bare-metal RISC-V machine to life. You will build and run the first version of EmbryOS: a tiny kernel that prints **“EmbryOS: Life Has Begun”** to the console using the RISC-V Supervisor Binary Interface (SBI).

The goal of this chapter is to set up the toolchain, understand how the system boots, and see how a kernel written in C and assembly runs under QEMU or real hardware.

## Learning Objectives

By the end of this chapter, you will understand:

- How to build EmbryOS using `riscv-none-elf-gcc` or `qemu-system-riscv32`
- How a linker script defines memory layout on a bare-metal system
- How assembly startup code initializes memory and sets up the stack
- How to print characters
- How to run EmbryOS
- How to use logging

## Kernel Modules

| File | Purpose |
|------|---------|
| `Makefile` | Describes how to build the EmbryOS kernel and run `qemu` |
| `start.S` | Entry point: clears `.bss`, sets the stack, jumps to `embryos_main()` |
| `hello.c` | Kernel main loop: prints a message and delays 1 second (approx.) |
| `log.c` / `log.h` | Logging support |
| `io.c` / `io.h` | Thin wrapper around `sbi_putchar()` |
| `sbi.S` / `sbi.h` | Defines the SBI `CONSOLE_PUTCHAR` ecall interface |
| `kprintf.c` / `kprintf.h` | Minimal formatted print routine for the kernel |
| `embryos.h` | Umbrella header for kernel interfaces |
| `platform.h` | Platform constants (e.g., `DELAY_MS`) |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `void sbi_putchar(int ch)` | Performs the SBI console output ecall |
| `void io_putchar(char c)` | Writes one character to the console |
| `void kprintf(const char *fmt, ...)` | Kernel printf with limited specifiers |
| `int embryos_main(void)` | Prints a message and sleeps in a loop |
| `void user_delay(int ms)` | Busy-wait delay calibrated by `DELAY_MS` |
| `stack_end` | Symbol defined in `start.s` marking the top of the 8 KiB kernel stack |

## Discussion

You can run the system under QEMU as follows: `make qemu`.
This should print `EmbryOS: Life Has Begun` periodically.

RISC-V defines three privilege levels: M(achine)-mode, S(upervisor)-mode, and U(ser)-mode.
M-mode is intended for firmware software.  EmbryOS, like Linux, relies on OpenSBI for
this, a thin layer of software that provides some essential services to operating system
kernels.  EmbryOS, like Linux, runs in S-mode.  U-mode is used for user processes, which
will not be introduced until Chapter 6.

When the machine boots, OpenSBI runs first in M-mode preparing the machine.  Then, it invokes 
`_start` in `start.S` in S-mode.  This code first clears the BSS and then invokes `embryos_main()`
in `hello.c`.  The only global variable at this time is the kernel stack defines in `start.S`.
Before moving on to Chapter 2, make sure you understand what's going on.  You can use your
favorite LLM to ask as many questions as you like.

## Check the Log

EmbryOS comes with fine-grained logging system built in.
The first line that EmbryOS prints is something like this:
`pmemsave 0x80200000 0x312000 mem.bin`.  This is a QEMU command.
When running EmbryOS under QEMU, you should copy this line and
then do the following to extract the log (post-morten):

- `<control>A c`: put QEMU in command mode
- `stop`: stops EmbryOS
- `pmemsave X Y mem.bin`: paste the command you copied to save the image
- `quit`: quit out of QEMU
- `./logdump qemu.elf mem.bin`: print the log

This will output something like this:

```
[0,0,0] BOOT
[1,0,0] IO_PUTCHAR c:'p'
[1,0,1] IO_PUTCHAR c:'m'
[1,0,2] IO_PUTCHAR c:'e'
[1,0,3] IO_PUTCHAR c:'m'
...
```

## 💬 Ask Your LLM

- Can you explain to me why `_start` must clear the `.bss` section before calling `main`?
- Where is the stack placed in memory, and how is it defined in assembly?
- What would happen if the stack were misaligned before the first function call?
- How does the SBI `CONSOLE_PUTCHAR` ecall reach QEMU’s terminal output?

## Exercise

Modify `hello.c` so the kernel prints a counter that increments once per approximately
one second.
