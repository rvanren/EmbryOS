# A Unix-Like File System (UFS)

## Overview

In this project, you will replace the existing `bd_simple` file system in EmbryOS with a **Unix-like file system (UFS)**. Your implementation will manage persistent storage at the **block level**, using i-nodes, indirect blocks, and a free-block list.
The file system must conform exactly to the **block device interface** defined in `bd.h`.

This project emphasizes:

* On-disk data structure design
* Block allocation and reclamation
* Multi-level indexing (direct, indirect, double indirect)
* Correct handling of sparse files (“holes”)

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

Your project directory must include:

```
ufs/
├── README.md        # Design explanation
├── bd_ufs.c         # File system implementation
├── bd_ufs.h
```

Your README must describe:

* Superblock layout
* i-Node structure
* Block allocation strategy
* Free-list organization
* How holes are represented and handled

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
