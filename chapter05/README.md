# Chapter 05: Keyboard Input

Applications often require input from a keyboard and need to block while
they are waiting for input.  This requires hardware-dependent interaction with
the UART.

## Learning Objectives

- Learn about UART types and how to discover them.
- Learn about how to see if there is input available and how to retrieve it.
- See how to implement a `user_get()` system-call interface to wait for input.
- See how EmbryOS tracks *keyboard focus* to support multiple processes that
  require input.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `uart.[ch]` | Per-UART type information and primary UART |
| `uart_*.c` | UART-specific interfaces |
| `process.h` | Keyboard input state of a process |
| `io.[ch]`, `io2.c` | Handling of keyboard input and interface to processes |
| `interrupt.c` | Interrupt handling |
| `sched2.c` | Scheduling |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct pcb *io_focus` | Process that currently has the keyboard focus (if any) |
| `struct pcb *io_wait` | Processes that are waiting for input but have no focus |
| `fdt_uart_info()` | Finds out information about all the available UART types |
| `uart_init(void)` | Determines the primary UART |
| `io_received(c)` | Invoked when somebody types `c` |
| `io_get(self, row, col, cf, cu)` | Shows cursor and waits for the next input character |
| `io_exit(self)` | Removes focus from a dead process |
| `user_get(row, col, cf, cu)` | User process system call to invoke `io_get()` |

## Discussion

Applications often need keyboard input.  There may be multiple such applications
at the same time.  For this, we introduce a *keyboard focus*: at most one
process can have the focus.  To switch between processes that want the focus,
type the TAB character.  When a process tries to read a character for the first
time, it receives the focus automatically; when a process with the focus dies,
its focus is removed and no process will have the focus.
If a process is having the focus and it is waiting for input, it displays the
`cf` cursor.  If a process is waiting for input but it does not have the
focus, then it displays the `cu` cursor at the given position.

Keyboard input is read on one of the UARTs.  Different platforms have different
types and number of UARTs, and they are mapped at different base addresses in
memory. `uart_init()` determines the types of UARTs, selects the first
active one it finds as the primary UART for input, and invokes its initialization
function.

The `isr` function of the primary UART is invoked to retrieve characters
from the UART.  It is non-blocking: it tries to retrieve as many characters
as possible, invoking `io_received()` for each one.  Since not all RISC-V platforms
support external interrupts, EmbryOS instead invokes the `isr` function
on any interrupt and exception (including timer interrupts) and also in the
idle loop of the scheduler.

Note that the PCB of a process is always in exactly one location: the `tp`
register of the hart if it is currently running; the run queue if it is
waiting for the CPU to become available, `io_wait` in case it wants input
but does not have the focus, or `io_focus` in all other cases.

## Check the Log

Run EmbryOS and extract the log as before. Now pay attention to the `IO_`
events:

```
% ./logdump qemu.elf mem.bin | grep IO_ | head -12
[6,4,14190] IO_GET self:0x8051c000 block:1 buffer:0 focus:0x0
[6,7,15220] IO_ADD pcb:0x8051c000 c:LOST_FOCUS buffer:0 waiting:1
[6,12,60887] IO_GET self:0x8051c000 block:1 buffer:0 focus:0x80520000
[7,29,757429] IO_RECEIVED focus:0x80520000 c:'w'
[7,30,757429] IO_ADD pcb:0x80520000 c:'w' buffer:0 waiting:0
[7,34,921768] IO_RECEIVED focus:0x80520000 c:'w'
[7,35,921768] IO_ADD pcb:0x80520000 c:'w' buffer:0 waiting:0
[7,46,1416931] IO_RECEIVED focus:0x80520000 c:<TAB>
[7,47,1416931] IO_ADD pcb:0x80520000 c:LOST_FOCUS buffer:0 waiting:0
[7,48,1416931] IO_ADD pcb:0x8051c000 c:GOT_FOCUS buffer:0 waiting:1
[7,50,1416960] IO_GET self:0x8051c000 block:1 buffer:0 focus:0x8051c000
[7,52,1417039] IO_GET self:0x80520000 block:1 buffer:0 focus:0x8051c000
```

In this case, I typed `ww<TAB>`, which you can see clearly in the log.
You can also see how typing `<TAB>` switched the focus to another process,
which uses non-blocking I/O to read input. The focus switched from the
`snake` process to the `pong` process in this case.

## 💬 Ask Your LLM

- How does `io_get()` block a process without consuming CPU time?
- What ensures that characters are delivered to exactly one process?
- How might this design be generalized to support multiple input devices?

## Exercises

- Add a `user_timed_get(row, col, fc, fu, time)` system call, which is like
  `user_get` but stops waiting at the given time.
- Write an interactive game using `user_timed_get()`.
