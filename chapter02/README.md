# Chapter 02: Applications and the Screen

An operating system distinguishes *applications* from the *kernel*.  The
kernel provides convenient *abstractions* to its applications. The most
important abstraction is the *screen*.

## Learning Objectives

- Differentiate between the kernel, which provides system services, and an application, which uses those services.
- Understand the design of a simple *screen* abstraction implemented as a matrix of colored character cells.
- Learn how user code communicates with the kernel through a narrow library interface (syslib.h).
- Create an application that draws graphics by invoking system calls.
- Observe how ANSI escape sequences are used to control cursor position and color on a terminal.
- Optionally: Use the *block pixel* abstraction for graphics

## New or Modified Modules

| File | Purpose |
|------|---------|
| `hello.c` | Now acts as the kernel main: initializes the screen, then launches an application (blockdemo) |
| `screen.c` / `screen.h` | Implements the screen abstraction: functions to draw individual *cells* or fill regions |
| `shared/syscall.h` | Defines `CELL()` |
| `die.c` / `die.h` | Exception handling |
| `apps/syslib.h` | Defines the application interface: inline `user_put()` and `user_delay()` |
| `apps/blockpixel.[ch]` | Block pixel abstraction for graphics |
| `apps/blockdemo.c` | A first user application that demonstrates block pixels |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `void screen_put(int col, int row, cell_t cell)` | Draws a single cell on the screen at the given coordinates |
| `void screen_fill(int x, int y, int w, int h, cell_t cell)` | Fills a rectangular region with the given cell value |
| `void user_put(int col, int row, cell_t cell)` | Application-side wrapper that issues a system call to draw a cell |
| `void blockdemo_main(void)` | Application entry point |
| `void die(void *msg)` | Prints `msg` and stops the kernel |
| `bp_init(bp, x, y, width, height, buffer)` | Initializes a block pixel grid |
| `bp_put(bp, x, y, color, enum bp_mode mode)` | Updates a block pixel |
| `bp_flush(bp)` | Flushes all *lazy* block pixel updates to the screen |

## Discussion

The most familiar output device of an operating system is a `screen`, by which
is meant a 2-dimensional matrix of pixels.  Unfortunately, today's hardware is
extremely complicated and diverse, and so EmbryOS avoids it entirely.  EmbryOS
relies instead on two commonly available features.  First is the UART
(Universal Asynchronous Receiver/Transmitter), which is a hardware device
that can send and receive 8-bit characters.  Second is the *ANSI escape code*,
a standard for in-band signaling to control cursor location and other options on
video text terminals and terminal emulators.  Using these two features, an
EmbryOS screen is a two-dimensional matrix of *cells* instead of pixels.

EmbryOS defines three types of cells: ASCII, BLOCK, and BRAILLE.
An ASCII cell is used for printable ASCII characters such as `a`, `0`, or `$`.
A BLOCK cell splits a cell into two solid subblocks: the top block and the
bottom block.  BLOCK cells are used for simple graphics.
A BRAILLE cell is a cell with a 2x4 dot matrix inside of it.
Each cell has a foreground color and a background color.
This chapter also introduces the user-level *block pixel* abstraction
for graphics that uses the BLOCK cell to create a two-dimensional matrix
of small colored blocks.

## Check the Log

Run EmbryOS and extract the log as before.  The most important new
type of event is USER_PUT:

```
% ./logdump qemu.elf mem.bin | grep USER_PUT | head -5
[1,34,210665] USER_PUT col:34 row:9 cell:CELL(BLOCK,green,red,0x01)
[1,34,210679] USER_PUT col:35 row:9 cell:CELL(BLOCK,yellow,green,0x01)
[1,34,210693] USER_PUT col:36 row:9 cell:CELL(BLOCK,blue,yellow,0x01)
[1,34,210707] USER_PUT col:37 row:9 cell:CELL(BLOCK,magenta,blue,0x01)
[1,34,210721] USER_PUT col:38 row:9 cell:CELL(BLOCK,cyan,magenta,0x01)
```

This shows the system calls that the `blockdemo` app makes.
See `shared/syscall.h` for more information on what a `CELL` is.

## 💬 Ask Your LLM

- How does the kernel separate application code from its own code in this chapter?
- What is a cell_t, and why does it encode both character and color?
- How do ANSI escape sequences implement cursor positioning and color changes?
- What would change if the screen were memory-mapped hardware instead of a terminal?

## Exercise

Design your own demo.
