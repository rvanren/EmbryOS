# Chapter 06: User Space and Virtual Memory

So far, the kernel and the applications have been running side-by-side
with no protection boundaries.  Here we are going to leverage RISC-V's 
U-mode and page tables.

## Learning Objectives

- Introduce the privilege boundary between kernel and user space on RISC–V.
- Learn how to enter user mode safely and return through the trap mechanism.
- Understand how the RISC–V page-table mechanism supports virtual memory.
- Learn how each process in EmbryOS now has its own page table and private address space.
- See how page faults are handled through on-demand mapping.
- Observe how user isolation and demand paging emerge naturally from the same mechanisms.
- Study the interaction between the virtual memory module, exception handling, and process management.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `app_info.h` | Information about embedded executables |
| `apps_gen.c` | Auto-generated file with list of encoded executables |
| `apps.c` | Starting a user process |
| `usertrap.S` | Handling interrupts that occur in user space |
| `interrupt.c` | Added page fault delegation to exception handler |
| `procsys.c` | Page fault handling |
| `vm.h`, `vm*.c` | Virtual memory module |
| `syscall.c` | System call handling |
| `shared/syscode.h` | System call codes |
| `apps/crt0.S` | Phase 1 of entry for user processes |
| `apps/crt1.c` | Phase 2 of entry for user processes |
| `apps/init.c` | The first process to run |
| `apps/dir.[ch]` | Listing of files/executables |
| `apps/syslib.[Sh]` | System call functions |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct embedded_file embedded_files[]` | NULL-terminated list of embedded files |
| `exec_user()` | Initializes a user process |
| `enter_user(start, end, sp)` | Executes a user process with interrupts enabled |
| `void syscall_handler(tf)` | Handles system calls |
| `void _start()` | Process start-up code |
| `void _crt_init(tf)` | C runtime initialization |
| `void user_exit()` | A system call to terminate a user process |
| `void proc_pagefault(tf)` | Invoked on a page fault with the trap frame |
| `void proc_miss(self, va)` | Handles a page fault at virtual address `va` |
| `void vm_init(hart)` | Initializes the virtual memory for the given hart |
| `void vm_flush(hart, base)` | Sets the page table to base and flush the TLB |
| `void vm_map(base, va, frame)` | In page table `base`, map virtual address `va` to `frame` |
| `int vm_is_mapped(base, uintptr_t va)` | Checks if `va` is currently mapped in `base` |
| `void vm_release(base)` | Release all the frames in page table `base` |

## Discussion

RISC-V has U-mode to support isolated user processes and the `ecall` instruction
that allows user processes to make calls to the kernel.  Each application is now
separately compiled into an executable.  Each application can now have its own
global variables, which was not possible before.

The first problem we face is how the kernel has access to the application code,
which used to be just linked with the kernel as additional modules.
We introduce a tool called `embedfiles` that generates a C file called
`apps_gen.c` containing a list of files as byte arrays.  We compile this
file into the kernel.  The first executable in this list is the `init`
process, which in turn spawns other processes.  Executables are numbered
starting at 2.

RISC-V only supports multi-level page tables.  EmbryOS only uses a *single
leaf table* for user processes, effectively simulating a one-level page table.
For 32-bit RISC-V, this page table has 1024 entries, while on 64-bit RISC-V
this page has only 512 entries.  The user virtual range can be found in `platform.h`.  
The rest of the virtual address space is mapped 1-1 to allow the kernel access 
to its own memory and the device registers.

There are two different trap handlers: one for interrupts in kernel code,
and one for interrupts in user code.  The latter requires access to the interrupted
process' kernel stack pointer, which is held in the `sscratch` register (look at 
`usertrap.S`).  When an interrupt occurs, `_user_trap_handler` restores the kernel 
stack pointer and also recomputes what the pointer to the PCB of the current process 
is, which it then must store in the `tp` register.  Note that device interrupts are 
only enabled when running user code or when running in the idle loop of `sched_idle()`.  
In the rest of the kernel code, device interrupts are masked.

## Check the Log

Run EmbryOS and extract the log.

```
% ./logdump qemu.elf mem.bin
[0,0,0] BOOT self:0x0
[1,0,0] UART_INIT self:0x0 idx:0 base:0x10000000
[2,0,31594] FRAME_INIT self:0x0 nframes:31450
[3,0,31617] TIME_BASE self:0x0 timebase:10000000
[4,0,31617] FRAME_ALLOC self:0x0 f:0x80522000
[4,1,31617] HART_INIT self:0x80522000 hart:4 idx:0
[5,1,31617] FRAME_ALLOC self:0x80522000 f:0x80523000
[5,2,31617] FRAME_ALLOC self:0x80522000 f:0x80524000
[5,3,33811] FRAME_ALLOC self:0x80522000 f:0x80525000
[5,4,33811] CTX_START self:0x80525000 old:0x80522000 new:0x80525000 hart:4 exec:2
[5,5,33811] FRAME_ALLOC self:0x80525000 f:0x80526000
[5,6,33812] INTERRUPTS_WORK self:0x80525000 hart:4
[6,6,33813] SCHED_IDLE self:0x80522000 hart:4
[7,6,33815] PAGEFAULT self:0x80525000 va:0x70000000
[7,7,33815] FRAME_ALLOC self:0x80525000 f:0x80527000
[7,8,33815] VM_MAP self:0x80525000 base:0x80526000 va:0x70000000 frame:0x80527000
[7,9,33816] PAGEFAULT self:0x80525000 va:0x701ffff8
[7,10,33816] FRAME_ALLOC self:0x80525000 f:0x80528000
[7,11,33816] VM_MAP self:0x80525000 base:0x80526000 va:0x701ffff8 frame:0x80528000
[7,12,33817] USER_SPAWN self:0x80525000 app:5
...
```

You can see how a process was started (`CTX_START`). Executable 2 is always the
`init` process.  It experiences two page faults right off the bat.
Virtual address `0x70000000` is the initial program counter.
Virtual address `0x701ffff8` is the initial stack access.
The `init` process then spawns another process (in executable 5) using the first
actual system call.

## 💬 Ask Your LLM

- How does setting the U bit in a page table entry restrict access to user mode?
- Why must `sret` be used instead of a normal jump to enter user mode?
- Why does `usertrap.S` need to restore the tp register on user traps?
- How does the user runtime (crt0.S, crt1.c, user.lds) simplify process startup?
- How does the hardware determine when to trigger a page-fault trap?
- Why does each process need its own page table, rather than sharing one global mapping?
- How does the kernel safely access user memory when handling a system call?
- What are the roles of the root_pt and mid_kernel structures in SV39 mode?

## Exercise

- Implement the system calls you have implemented in prior projects as
  *proper* system calls using the `ecall` instruction.
