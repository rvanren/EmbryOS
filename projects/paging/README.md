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
The new virtual address space should be at least a Gigabyte.
Thus, since `VM_START` is defined to be 0x70000000, `VM_END`
should be at least 0x80000000.

When successful, you will be able to compile the 64-bit version
of the selfie compiler with itself.  The 32-bit version already
can, but 2 MB is not enough for the 64-bit version to compile itself.
For example, selfie can compile `echo.c` (`selfie -c echo.c -o echo`),
but when you try to compile the selfie compiler with itself
(`selfie -c selfie64.c -o selfie64`) the compiler will run out of
memory.

---

## Block Device Interface

Your implementation must implement the following interface from `bd.h`:

```c
#define BLOCK_SIZE ...

struct block {
    char bytes[BLOCK_SIZE];
};

struct bd {
    void *state;
    int  (*alloc)(void *state);                 // allocate an inode
    int  (*size)(void *state, int inode);       // maximum size in blocks
    void (*read)(void *state, int inode, int blk, void *dst);
    void (*write)(void *state, int inode, int blk, const void *src);
    void (*free)(void *state, int inode);       // free an inode
};

extern const struct block bd_null_block;
```

### Important Semantics

* Each inode represents a **file**
* Each file is a fixed-size array of blocks
* All blocks are initially **zero-initialized**
* Writing must allocate blocks on demand

---

## On-Disk Layout

The file system is laid out as follows:

```
Block 0        : Superblock
Block 1..N     : i-node blocks
Remaining      : Data blocks, indirect blocks, free list blocks
```

---

## Superblock (Block 0)

The superblock contains:

* The number of i-node blocks
* A pointer to the **first free-list block**

You are free to define the exact layout of the superblock, but it must at least encode this information.

---

## i-Node Blocks

* Each i-node block contains a **fixed number of i-nodes**
* Each i-node consists of **exactly four 32-bit words**:

| Word | Meaning                                    |
| ---: | ------------------------------------------ |
|    0 | Allocation flag (allocated or free)        |
|    1 | Pointer to first data block (direct block) |
|    2 | Pointer to an indirect block               |
|    3 | Pointer to a double-indirect block         |

### Notes

* The allocation flag is currently just a boolean
* A null pointer represents a **hole**
* Files may be sparse

---

## Indirect and Double-Indirect Blocks

### Indirect Block

* Contains an array of **32-bit block pointers**
* Points directly to data blocks

### Double-Indirect Block

* Contains pointers to **indirect blocks**
* Each of those indirect blocks points to data blocks

This allows files to grow well beyond what fits in direct pointers.

---

## Free List

Free blocks are managed using a **linked list of free-list blocks**.

### Free-List Block Format

* Same layout as an indirect block: an array of 32-bit block pointers
* **First pointer** points to the **next free-list block**
* Remaining pointers point to free blocks (unless null)
* A free-list block may not be full
* A null pointer also indicates **end of the free list**

---

## Required Behavior

Your UFS implementation must:

* Allocate blocks on demand when writing
* Reuse blocks when files are freed
* Correctly handle sparse files
* Return zeroed blocks when reading holes
* Never leak blocks
* Never allocate the same block twice

---

## Required Files

You must implement the file system in:

```
bd_ufs.c
bd_ufs.h
```

You must also update:

```
files.c
```

to use `bd_ufs` instead of `bd_simple`.

No other EmbryOS files should require modification.

---

## Required Functions

Your `bd_ufs` implementation must provide:

* `alloc` — allocate a new i-node
* `free` — free an i-node and all its blocks
* `size` — return the maximum file size in blocks
* `read` — read a block (handling holes)
* `write` — write a block (allocating as needed)

It should also provide a function
`void ufs_init(struct bd *iface, struct ufs_state *s,
                struct bd *lower, int inode_below, int n_inodes)`
to initialize the file system with at least the given number of
i-nodes.

---

## Error Handling and Edge Cases

Your implementation must handle:

* Reading holes and writing into holes
* Files with no allocated blocks
* Partially filled free-list blocks
* Multiple levels of indirection
* Exhausted free space (graceful failure)

---

## Deliverables

Implement your code within `chapter12`.  Submit a tar file of this
directory, which you can create by running the following command in
the `chapter12` directory: `make clean; tar cf ../files.tar .`
Also submit `log/log_events.def`, `log/log_defs.h`, and `log/logdump.c` if you added log events (we encourage you to do so).

In `chapter12`, include a file called "explanation.md" which describes:

* Superblock layout
* i-Node structure
* Block allocation strategy
* Free-list organization
* How holes are represented and handled
* How you used AI, if at all

---

## Constraints

* No changes to the `bd.h` interface
* No changes to user-visible file APIs
* No busy waiting
* No memory leaks
* Clean, readable code

---

## Summary

This project implements the **core of a real file system**:

* Persistent metadata
* Multi-level block indexing
* Dynamic allocation
* Sparse file support

Correctness and clarity matter more than performance. Design carefully before you code—retro-fitting correctness into a file system is difficult.
