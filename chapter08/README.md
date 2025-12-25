# Chapter 08: Block Storage

Some state of operating systems should survive reboots.  Block-based storage
prepares the ground for representing files as arrays of bytes.

## Learning Objectives

- Introduce the block-device abstraction as a uniform interface for block-addressable storage.
- Understand how block-device layers can be stacked to separate allocation from access.
- Implement a RAM-disk device that provides raw block storage.
- Build a simple block layer that manages allocation and indirection through inodes.
- Develop a unit test to discover a bug in the SIMPLE block layer
- Use the log to find the location of the bug

## New or Modified Modules

| File | Purpose |
|------|---------|
| `bd.h` | Defines the block device abstraction |
| `bd_ramdisk.c` | Emulation of a disk device on RAM |
| `simple*.c` | Implementation of the SIMPLE file system |
| `files.c` | Initialization of a file system and unit test |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct block ramdisk[]` | RAM disk storage space |
| `struct bd ramdisk_iface` | RAM disk interface |
| `struct ramdisk_state ramdisk_state` | RAM disk meta state |
| `struct bd simple_iface` | SIMPLE disk interface |
| `struct simple_state simple_state` | SIMPLE disk meta state |
| `int (*alloc)(state)` | allocate an inode |
| `int (*size)(state, inode)` | maximum size in blocks |
| `void (*read)(state, inode, int blk, dst)` | read a block |
| `void (*write)(state, inode, int blk, src)` | write a block
| `void (*free)(state, inode)` | free an inode |
| `ramdisk_init(iface, state, mem, nblocks)` | initialize a RAM disk |
| `simple_init(iface, state, bd_below, inode_below, format)` | initialize a SIMPLE disk |
| `void files_init(void)` | initialize the file system stack |

## Discussion

This chapter introduces persistent block management through a pair
of stacked block device layers.  A block device defines the `inode`
abstraction: a fixed size sequence of blocks.
Each block consists of `BLOCK_SIZE` bytes.
This provides a minimal form of storage without yet introducing
files or directories.

The lower layer, implemented in `bd_ramdisk.c`, provides raw block
storage using a fixed in-memory array.  Reads and writes copy blocks
between this array and the caller’s buffer. The interface hides the
details of the underlying memory representation and exposes uniform
block operations.  While it is certainly possible to write an
interface to a real disk instead, there are many available storage
devices and doing so in a portable way is all but impossible. Hence
the choice of a RAM disk for EmbryOS.

The upper layer, implemented in `bd_simple.c` and `bd_simple_ops.c`,
adds structure on top of the RAM disk. It maintains a free-block
list for allocation and a collection of inodes, each of which serves
as an array of block numbers. Every inode maps its logical block
indices to physical blocks in the underlying device. Inodes are
identified by the block number that contains their indirect block.
When initialized with the format flag set, the SIMPLE layer creates
an initial free-block list by chaining all unused blocks together.
Allocation removes the head of the list; freeing a block pushes it
back.

The function `files_init()` builds the stack by connecting the
`ramdisk_iface` as the lower layer and the `simple_iface` above it.

## Check the Log

The unit test in `files.c` fails, complaining about a full disk.
This is good news: we purposely added a bug to the file system.
The test repeatedly writes an entire file and then deallocates it.
If you look at the log, you will be able to distinguish various phases.
In the first phase, the free list of the SIMPLE block layer is initialized:

```
[4,0,218] SIMPLE_FORMAT_START below:0 nblocks:1024
[4,1,219] RAMDISK_WRITE inode:0 block:1023 src:0x80232858
[4,2,219] RAMDISK_WRITE inode:0 block:0 src:0x80232858
[4,3,220] RAMDISK_WRITE inode:0 block:1022 src:0x80232858
[4,4,220] RAMDISK_WRITE inode:0 block:0 src:0x80232858
...
[4,2045,1240] SIMPLE_FREE_BLOCK b:1
[4,2045,1241] RAMDISK_WRITE inode:0 block:1 src:0x80232858
[4,2046,1241] RAMDISK_WRITE inode:0 block:0 src:0x80232858
[4,2047,1241] SIMPLE_FORMAT_END
```

Next, the unit test start with the first run:
```
[4,2048,1241] UT_SIMPLE_RUN run:0
[4,2049,1241] SIMPLE_ALLOC_BLOCK b:1
[4,2049,1242] RAMDISK_READ inode:0 block:1 dst:0x80232858
[4,2050,1242] SIMPLE_ALLOC inode:1
[4,2051,1242] RAMDISK_WRITE inode:0 block:1 src:0x80209f20
[4,2052,1242] SIMPLE_SIZE inode:1
```

Then the first run completes:
```
[4,2053,1242] UT_SIMPLE_START run:0 ino:1 size:512
[4,2054,1242] UT_SIMPLE_CHK1 run:0 k:0
[4,2054,1243] SIMPLE_READ inode:1 block:0 dst:0x80233058
[4,2055,1243] RAMDISK_READ inode:0 block:1 dst:0x80231020
...
[4,7176,3290] RAMDISK_READ inode:0 block:1 dst:0x80232020
[4,7177,3290] SIMPLE_FREE_BLOCK b:1
[4,7177,3291] RAMDISK_WRITE inode:0 block:1 src:0x80232858
[4,7178,3291] RAMDISK_WRITE inode:0 block:0 src:0x80232858
```
Then the second run starts:
```
[4,7179,3291] UT_SIMPLE_RUN run:1
[4,7180,3291] SIMPLE_ALLOC_BLOCK b:1
[4,7180,3292] RAMDISK_READ inode:0 block:1 dst:0x80232858
[4,7181,3292] SIMPLE_ALLOC inode:1
[4,7182,3292] RAMDISK_WRITE inode:0 block:1 src:0x80209f20
[4,7183,3292] SIMPLE_SIZE inode:1
[4,7184,3292] UT_SIMPLE_START run:1 ino:1 size:512
[4,7185,3292] UT_SIMPLE_CHK1 run:1 k:0
[4,7185,3293] SIMPLE_READ inode:1 block:0 dst:0x80233058
[4,7186,3293] RAMDISK_READ inode:0 block:1 dst:0x80231020
[4,7187,3293] UT_SIMPLE_CHK2 run:1 k:0
[4,7187,3294] SIMPLE_WRITE inode:1 block:0 src:0x80233058
[4,7188,3294] RAMDISK_READ inode:0 block:1 dst:0x80231820
[4,7189,3294] SIMPLE_ALLOC_BLOCK b:514
```
But it does not end well:
```
[4,10757,4824] SIMPLE_WRITE inode:1 block:510 src:0x80233058
[4,10758,4824] RAMDISK_READ inode:0 block:1 dst:0x80231820
[4,10759,4824] SIMPLE_ALLOC_BLOCK b:0
[4,10759,4825] DIE self:0x80534000
```
If you track the allocation and release of blocks, you
will be able to tell what the problem is and find the location of
the bug.

## 💬 Ask Your LLM

- What are the advantages of defining a uniform block-device interface?
- Why is each inode stored as an ordinary block within the same layer?
- What properties make this design easy to extend with additional layers?
- How could the same abstraction support a persistent device instead of a RAM disk?

## Exercise
Write a test that allocates an inode, writes several blocks through
it, frees a subset, and verifies that subsequent allocations reuse
the freed blocks.
