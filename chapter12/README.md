# Chapter 12: Fancy Applications

To demonstrate the EmbryOS can support sophisticated applications, this chapter
introduces implementation of a memory allocator, a standard file interface,
a text editor, and a 64-bit RISC-V compiler for a subset of C.

## Learning Objectives

- How `malloc()` memory allocation works.
- That's it--you pretty much have learned all there is to know.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `apps/malloc.c` | Memory allocation |
| `apps/stdio.[ch]` | File interface |
| `apps/ed.c` | A rudimentary text editor |
| `selfies/selfie.c` | A compiler for C*, a subset of C |
| `selfies/echo.c` | A C* program that prints its arguments |
| `selfie.h`, `selfies.c` | Kernel support for selfie |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `void *malloc(size)` | Allocates a chunk of memory |
| `free(void *ptr)` | Releases an allocated chunk of memory |

## Discussion

This chapter is mostly to demonstrate that the operating system that we have built
is capable or running all kinds of applications.  Of course, building a mature
operating system requires much more work.  You have seen various examples of UART
drivers.  There are many more, and then there are disk drivers, USB drivers,
network drivers, screen drivers, clock drivers, etc.  Also, most operating systems
are portable to other hardware platforms than RISC-V.

An important part of an operating system lives in user space.  Libraries for heap
management (`malloc`) and input/output (`stdio`) not only make programming
applications easier, but they can also help with portability and efficiency.
The `init` process can also be considered part of the operating system, and
usually there are many more such processes that support operating system functions
like scheduling tasks (`cron`), login, etc.

## Check the Log

Below is the result of running `echo hello` after first running
`selfie -c echo.c -o echo` to compile the `echo` program.
First you see the events related to paging in the program, followed
by printing "echo" and <NEWLINE>, followed by `exit`.

```
[21,6727,135877] SELFIE_INIT self:0x805e2000 brk:0x11018
[21,6728,135877] SELFIE_BREAK self:0x805e2000 brk:0x0
[21,6729,135895] SELFIE_BREAK self:0x805e2000 brk:0x11018
[21,6730,135901] PAGEFAULT self:0x805e2000 va:0x11010
[21,6732,135901] FLAT_READ file:16 off:8192 dst:0x805fc000 n:4096
[21,6733,135901] SIMPLE_READ inode:1 block:0 dst:0x802c38c0
[21,6734,135901] RAMDISK_READ inode:0 block:1 dst:0x802c20c0
[21,6735,135901] RAMDISK_READ inode:0 block:2 dst:0x802c38c0
[21,6736,135901] SIMPLE_READ inode:892 block:4 dst:0x802c50c0
[21,6737,135901] RAMDISK_READ inode:0 block:892 dst:0x802c20c0
[21,6738,135901] RAMDISK_READ inode:0 block:897 dst:0x802c50c0
[21,6739,135901] VM_MAP self:0x805e2000 base:0x805e3000 va:0x11010 frame:0x805fc000
[21,6740,135917] SELFIE_WRITE self:0x805e2000 fd:1 src:0x1ffff8 size:8
[21,6743,135944] SELFIE_WRITE self:0x805e2000 fd:1 src:0x11000 size:1
[21,6744,136453] SELFIE_EXIT self:0x805e2000 status:0
```

## 💬 Ask Your LLM

- Why is `malloc()` implemented as a user-space library rather than a kernel system call?
- How does the `stdio` interface abstract over the underlying file system operations?
- What are the trade-offs between implementing functionality in user space versus the kernel?
- Why does the `selfie` compiler require significant virtual memory, and how does this relate to the page fault mechanism?
- How does the separation between kernel services (like file I/O) and user libraries (like `stdio`) improve portability?

## Exercise

- In theory, `selfie` can compile itself.  However, we do not currently provide
  enough virtual memory for it to do so.  Extend virtual memory from 2 MiB to 1 GiB.
