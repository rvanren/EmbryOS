# A Bigger Virtual Address Space

## Overview

In this project, you will extend the virtual address space.
The size of the virtual address space is defined by the
constants `VM_START` and `VM_END` in `platform.h`.
The total size depends on whether it is the 32-bit architecture
(1024 pages or 4 MB) or the 64-bit architecture (512 pages or 2 MB).
This is because a page index in the 32-bit architecture is 10
bits, but only 9 bits in the 64-bit architecture.  However,
the 64-bit architecture has more levels and can support larger
address spaces.  Out of the box, EmbryOS only supports 1 page
table level.  In this project, you will extend it to 2 or more.
The new virtual address space should be at least 256 MB.
Thus, since `VM_START` is defined to be 0x70000000, `VM_END`
should be at least 0x80000000.

When successful, you should be able to compile the 64-bit version
of the selfie compiler with itself.  The 32-bit version already
can, but 2 MB is not enough for the 64-bit version to compile itself.
For example, selfie can compile `echo.c` (`selfie -c echo.c -o echo`),
but when you try to compile the selfie compiler with itself
(`selfie -c selfie64.c -o selfie64`) the compiler will run out of
memory with only 2 MB of virtual memory.

---

## Virtual Memory

On 32-bit machines, we use all 32 bits of a word for virtual
addressing: 10+10+12 (2 page table levels with 1024 PTEs per page table).
On 64-bit machines, we currently use only 39 of the 64 bits for virtual
addressing: 9+9+9+12 (3 page table levels with 512 PTEs per page table).
Virtual memory is implemented in files `vm32.c` (32-bit) and `vm39.c`
(64-bit).  Study these (with the help of an LLM if you like) in order to
get familiar not only with the code but also the structure of RISC-V
page tables.  You may want to refer back to Chapter 06 where virtual
memory was first introduced.

## Deliverables

Implement your code within `chapter12`.  Submit a tar file of this
directory, which you can create by running the following command in
the `chapter12` directory: `make clean; tar cf ../paging.tar .`
Also submit `log/log_events.def`, `log/log_defs.h`, and `log/logdump.c` if you added log events (we encourage you to do so).

In `chapter12`, include a file called "explanation.md" which describes
your paging implementation and how you used AI (if at all).
