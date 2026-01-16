# Chapter 03: Resource Discovery and Context Switching

An operating system multiplexes resources such as CPU time, memory,
and the screen.
In this chapter, we learn how EmbryOS finds out how much memory it has
available to run processes and how it manages this memory.  Moreover,
we learn how EmbryOS time-multiplexes the CPU to provide the illusion
of multiple processes running at the same time.

## Learning Objectives

- Use the Flattened Device Tree (FDT) to discover the top of physical memory at boot time.
- Introduce a simple kernel memory allocator based on fixed-size frames.
- Understand how the kernel represents, creates, and switches between multiple processes.
- Learn how cooperative multitasking is implemented through context switching and a run queue.
- Introduce the process control block (pcb) as the unit of execution and scheduling.
- See how the RISC–V tp register always points to the pcb of the currently running process, allowing constant–time access to per–process data.
- Study how the scheduler maintains two linked structures: a circular run_queue of runnable processes and a zombies list of terminated ones.
- Learn why a process cannot clean itself up and how the scheduler reclaims its resources.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `hello.c` | Initializes the free frame list and starts the `init` process. |
| `libfdt/fdt_embryos.c` | Discovers available resources |
| `frame.c` / `frame.h` | Manages available physical memory frames |
| `hart.h` | Info about the RISC-V `hart` (core) |
| `process.c` / `process.h` | Process Control Block management |
| `sched.c` / `sched.h` | Scheduling |
| `die.c` | Exception handling for processes |
| `shared/ctx.[Sh]` | Context switching |
| `apps/syslib.c` | Updated system call interface |
| `apps/init.c` | The executable of the first process to run |
| `apps/life.c` | Game of Life application |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct hart hart` | Info about the RISC-V core |
| `union free_frame frames[]` | Start of the frame region, defined by the linker script |
| `union free_frame *free_list` | Free list of memory frames |
| `struct pcb *run_queue` | A circular queue of all runnable processes except the one currently executing |
| `struct pcb *zombies` | A linked list of processes that have exited but whose memory has not yet been reclaimed |
| `void (*apps[])()` | An array of applications |
| `void frame_init(void)` | Sets up the free frame list |
| `void *frame_alloc(void)` | Allocates a frame and returns its address |
| `void frame_release(void *frame)` | Returns a frame to the free list |
| `proc_create(hart, executable, area, args, size)` | Allocates a Process Control Block for a new process |
| `proc_enqueue()` / `proc_dequeue()` | Manages circular queues of processes |
| `proc_put(pcb, row, col, cell)` | Puts cell in (row, col) in the area of pcb |
| `proc_release(pcb)` | Releases the resources of the given process |
| `sched_self()` | Returns the pointer to the current process, stored in tp |
| `sched_set_self(pcb)` | Sets tp to pcb, updating the current process |
| `void sched_run(executable, area, args, size)` | Starts a new process |
| `sched_block()` | Switches to the next process on the run queue |
| `sched_yield()` | Moves the current process to the back of the run queue and switches to the next |
| `sched_exit()` | Marks the current process as a zombie and yields to another process |
| `sched_idle()` | Idle loop of a hart |
| `exec_user()` | Entry point of a new process |
| `ctx_switch()` / `ctx_start()` | Assembly routines that save and restore registers and switch stacks |
| `void die(void *msg)` | Prints `msg` and stops the process |
| `user_yield()` | System call to yield to another process |
| `user_spawn(app, row, col, wd, ht, args, size)` | System call to spawn a new process |

## Discussion

This chapter, longer than most, introduces various new concepts.
First is the *frame*, a unit
of memory (4096 bytes) for allocation and deallocation.  Second is the *process*.
The kernel maintains a *Process Control Block* (PCB) for each process.
The register state of a running process is called its *context*.  Third is
the *scheduler*, managing the currently running process, a *run queue* of
processes that are waiting to run, and a *zombie queue* of processes that
have terminated and must be cleaned up.

Physical memory starts right after the kernel at `frames`
(defined in file `platforms/qemu/kernel.ld`).
To find the end of available memory, OpenSBI gives the kernel access to a
*Flat Device Tree* describing the hardware in great detail.
Function `fdt_memory_range(fdt, &base, &end)` returns the start and end of physical
memory. `frame_init(fdt, end)` then sets up the free list, taking care not to put
the `fdt` itself on the free list.

The RISC-V `tp` register points to the PCB of the current process.  The PCB
maintains what executable it is executing, the area of the screen it has access
to, the kernel `sp` in case it is not currently running, and a pointer to the
hart in case it is running.

Processes are time-multiplexed on a single hart.  To do so, the registers of
the current process must be saved on its own stack, then the stack pointer
must be saved in its PCB, then the stack pointer of the new process must be
restored, followed by its registers that are saved on its stack.  To start
a new process is similar, but instead of restoring the registers of the
new process (which cannot be done), the function `exec_user()` is invoked.
This function invokes the application code.

In `embryos_main`, the code turns itself into a process by calling
`sched_set_self`.  This code eventually invokes `sched_idle` which runs
an idle loop in case no other process is running.  Before that, it
spawns the `init` process, which in turn spawns the `splash` process
and two copies of `life` (Conway's Game of Life).

## Check the Log

Run EmbryOS and extract the log as before.

```
% ./logdump qemu.elf mem.bin
[0,0,0] BOOT
[1,0,0] UART_INIT idx:0 base:0x10000000
[2,0,96] FRAME_INIT nframes:7865
[3,0,118] HART_INIT hart:1 idx:0 self:0x80514000
[4,0,2261] CTX_START old:0x80514000 new:0x80518000 hart:1 exec:2
[4,1,2261] USER_SPAWN self:0x80518000 app:3
[4,2,2261] CTX_START old:0x80518000 new:0x8051c000 hart:1 exec:3
[4,3,4195] USER_DELAY ms:200
[4,4,4195] USER_YIELD self:0x8051c000
[4,5,4196] SCHED_IDLE self:0x80514000 hart:1
```

This is looking increasingly like the log of an operating
system.  Look for these events in the code to help you
understand what the operating system does and how it works.
You may be surprised that the vector timestamp jumps from
[1,0,0] to [2,0,96].  This is because each log level, including
the last one is bounded and circular.  Between UART_INIT and
FRAME_INIT, 96 IO_PUTCHAR events happened because of stuff that
got printed.  However, those last-level events are no longer
in the log because they got overwritten by later events.

## 💬 Ask Your LLM

- What information does the Flattened Device Tree provide to the kernel?
- How does the RISC–V tp register let kernel code find the current process?
- What would happen if a process tried to free its own stack before calling sched_exit()?
- How does the circular run queue ensure fairness among runnable processes?

## Exercises

- Add the block pixel demo of the last chapter to the upper left window.
- Create a system call to obtain the width and height of the process's screen area.
