# EmbryOS Event Logging

This directory contains the **event logging system** used by EmbryOS for
debugging, tracing, and understanding system behavior *after the fact*.

Instead of printing debug messages at runtime, EmbryOS records **structured
events** in memory and decodes them later using a host-side tool (`logdump`).
This makes it possible to inspect system behavior even after crashes or hangs.

This logging system is used throughout the kernel (boot, scheduler, traps,
context switches, I/O).

## What you get as a user

With logging enabled, you can extract memory from QEMU and run:

```
./logdump qemu.elf mem.dump
```

Example output:

```
[0,0,0] BOOT
[1,0,0] UART_INIT idx:0 base:0x10000000
[2,0,0] HART_INIT hart:0 idx:0
[3,0,0] CTX_START old:0x805c7000 new:0x805ca000 hart:0 exec:2
[10,4,1416] TRAP scause:'System call' sepc:0x1003c stval:0x0
```

Each line represents a **kernel event** with:

- a vector timestamp (`[base,norm,freq]`, see below)
- a symbolic event name
- named, typed arguments

## How logging is used in the kernel

The kernel logs events using lightweight helpers:

```c
L0(level, type);
L1(level, type, arg0);
L2(level, type, arg0, arg1);
... (up to L6 with 6 arguments)
```

There are three logging levels (represented in the vector timestamp):
```
L_BASE - very rare but important events
L_NORM - scheduler, processes
L_FREQ - extremely frequent events such as interrupts
```

Example:

```c
L1(L_FREQ, L_IO_PUTCHAR, c);
```

Logging is constant time and allocation-free.
Each log level is bounded (circular), so, in particular, old L_FREQ
events are likely lost.

## Event definitions (`*.def` files)

All log events are defined in `.def` files using a simple declarative format.
This file is the **single source of truth** for event names, IDs, argument names,
and argument types.

Example:

```c
LOG_EVENT(IO_PUTCHAR, "c", ARG_CHAR)
LOG_EVENT(TRAP, "scause:sepc:stval",
          ARG_CAUSE, ARG_PTR, ARG_PTR)
```

## How to add a new event

1. **Add it to a `.def` file**

```c
LOG_EVENT(PROC_CREATE, "pid:parent", ARG_UINT, ARG_UINT)
```

2. **Use it in the kernel**

```c
L2(L_NORM, L_PROC_CREATE, pid, parent_pid);
```

3. **Rebuild, run, dump, decode**

```text
$ make qemu
```
hit ^A c to get into qemu command mode
```
(qemu) stop
(qemu) pmemsave <kernel_base> <size> mem.dump
(qemu) quit
$ ./logdump qemu.elf mem.dump
```
(the `pmemsave` command can be copied and pasted from the bottom of the screen)

Your event will appear automatically.

## Summary

- Logs are structured events, not printf output
- Events are defined once, in `.def` files
- The kernel records raw data; tools add meaning
- Adding new events is easy and encouraged
