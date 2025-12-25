# Chapter 11: Multi-Core Support

Modern computers achieve speed not by increasing the clock speed but by
increasing the number of CPUs.

## Learning Objectives

- Understand how multiple harts (RISC–V cores) share kernel data structures safely.
- Learn how the big kernel lock (BKL) serializes kernel execution across CPUs.
- Study how secondary harts are started using `sbi_hart_start()`.
- Observe how the scheduler and interrupt system operate in a multicore environment.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `hello.c` | Kernel initialization |
| `hart.[ch]` | Hart management |
| `apps.c` | Start of a process must release BKL |
| `interrupt.c` | Interrupt handler should obtain and release BKL |
| `sched2.c` | Kernel idle loop should release and obtain BKL |
| `sbi.S` | Support for adding harts |
| `start.S` | Hart entry |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `hart_start_others(fdt)` | Used by boot hart to start remaining harts |
| `sbi_hart_start(hartid, hart_entry, pcb)` | start a hart |
| `hart_entry(hartid, self)` | Assembly entry point of new harts |
| `hart_main(hartid, self)` | C entry point of new harts |
| `hart_init(void)` | Initializes a hart |

## Discussion

This chapter extends EmbryOS to run on multiple hardware threads
(harts). Each hart executes the same kernel code but operates on
shared data structures such as the run queue and frame allocator.
To maintain consistency, all kernel code paths are protected by a
single global spinlock known as the Big Kernel Lock (BKL).

At boot, the primary hart initializes the system and starts
the other harts by calling `hart_start_others()`.  Each secondary
hart sets up its stack and calls hart_main(), which in turn calls
hart_init() and then enters the idle loop.

The BKL ensures that only one hart executes kernel code at any time,
while all others wait in user mode or sleep in wfi.  When an
interrupt occurs, the handler first acquires the BKL before
manipulating shared state.  Once the trap is serviced and the process
resumes in user mode, the lock is released. This guarantees safety
without requiring fine-grained synchronization.  The BKL
provides a simple, conservative concurrency model: it allows
multiple cores to run user code concurrently while the kernel itself
remains serial.

## Check the Log

Below are the first level-1 log events that show how the various
harts are incorporated:

```
[0,0,0] BOOT
[1,0,0] UART_INIT idx:0 base:0x10000000
[2,0,31414] FRAME_INIT nframes:31270
[3,1,31437] HART_INIT hart:1 idx:0 self:0x805d6000
[4,3,31488] SIMPLE_FORMAT_START below:0 nblocks:1024
[5,3,34557] SIMPLE_FORMAT_END
[6,3,34557] FLAT_INIT format:1
[7,3553,39521] HART_INIT hart:2 idx:1 self:0x805d7000
[8,3555,39521] SCHED_IDLE self:0x805d7000 hart:2
[9,3555,39522] SCHED_IDLE self:0x805d6000 hart:2
[10,3555,39523] HART_INIT hart:3 idx:2 self:0x805dc000
[11,3557,39523] SCHED_IDLE self:0x805dc000 hart:3
[12,3557,39525] INTERRUPTS_WORK hart:1
[13,3557,39526] INTERRUPTS_WORK hart:2
[14,3557,39527] INTERRUPTS_WORK hart:3
...
```

The first hart to grab the BKL is the only hart that invokes
`embryos_main`.  It sets up the UART the free list of frames,
and the file system.
Eventually it releases the BKL to run the `init` process.
At that point, another hart comes in, but it will invoke
`hart_main`.  The same happens for yet another hart.

## 💬 Ask Your LLM

- Why is it safe for user processes to run on multiple cores while the kernel is serialized?
- What would be required to replace the Big Kernel Lock with finer-grained locks?
- Why must the idle loop release the BKL before executing `wfi`?
- How could scheduling be extended to balance work across harts?

## Exercise

Instrument the kernel to measure how often harts contend for the BKL.
Add a counter to spinlock_acquire() and display statistics from each hart’s
idle loop. How frequently is the lock held versus released?
