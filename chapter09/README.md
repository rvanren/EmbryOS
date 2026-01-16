# Chapter 09: File System

Files as unstructured sequences of bytes are the primary long-term storage
abstraction provided by operating systems.

## Learning Objectives

- Extend the block–layer architecture to represent files as arrays of bytes.
- Understand how metadata describes each file’s size and inode location.
- Learn how to read, write, and delete files through layered block operations.
- Observe how executables are now stored and loaded as ordinary files.
- Recognize how layering simplifies file-system construction.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `flat.h` | Defines the flat file abstraction |
| `flat[12].c` | Implementation of the flat file abstraction on block devices |
| `stat.h` | Per file meta-data |
| `files.c` | Initialization of a file system |
| `procsys.c` | Paging from a file |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct flat flatfs` | State of the flat file system |
| `void flat_init(fs, bd, format)` | Initializes file system `fs` on top of block device `bd`. |
| `int flat_create(fs)` | Creates a new file and return file identifier |
| `int flat_size(struct flat *fs, int file)` | Gets the size of a file |
| `flat_read(fs, file, offset, dst, n)` | Reads part of a file |
| `flat_write(fs, file, offset, src, n)` | Writes part of a file |
| `flat_delete(fs, file)` | Deletes a file |
| `proc_miss(self, va)` | Page fault at virtual address `va` |

## Discussion

This chapter adds the flat layer, which turns arrays of blocks into
variable arrays of bytes.
The flat layer sits above the block device layer.
Each file is described by a persistent `struct stat_entry` record
stored in inode 1. A `stat_entry` contains the inode number of the
file’s block array and its size in bytes. Because inode 1 holds the
table of all stat_entry records, it acts as a simple directory
indexed by small integer *file numbers*.

Creating a file calls `flat_create()`, which allocates a new inode
in the lower layer, initializes a `stat_entry`, and stores it in the
first available slot of inode 1. Reading and writing use `flat_read()`
and `flat_write()`, which translate byte offsets into block numbers
and update the file’s size as needed.  Deleting a file frees its
inode and clears its `stat_entry`.

The kernel now uses this mechanism to store executables persistently.
At boot time, `files_init()` formats the storage layers, creates one
file per application, and writes each program’s image into it.  The
page fault handler `proc_miss()` is updated by reading the corresponding
file through the flat interface.

## Check the Log

There is much to learn from perusing the log.  Here are some fragments:

```
[4,4633,218] ADD_FILE i:0
[4,4634,218] RAMDISK_READ inode:0 block:515 dst:0x80232858
[4,4635,218] SIMPLE_ALLOC inode:515
[4,4636,218] RAMDISK_WRITE inode:0 block:515 src:0x8020a4e8
```
The first file added to the file system is the `init` executable.
You can see here that it is stored in inode 515.
The file number is 2 (as the stat_inode has file number of 0, and
the root directory has a file number of 1), so `init` has an index
of 2 in the stat_entry records.

```
[4,5167,33791] FRAME_ALLOC f:0x80538000
[4,5168,33791] CTX_START old:0x80536000 new:0x80538000 hart:1 exec:2
[4,5169,33791] FRAME_ALLOC f:0x80539000
...
[6,5171,33795] PAGEFAULT self:0x80538000 va:0x70000000
[6,5172,33795] FRAME_ALLOC f:0x8053b000
[6,5173,33795] FLAT_READ file:2 off:4096 dst:0x8053b000 n:4096
[6,5174,33795] SIMPLE_READ inode:1 block:0 dst:0x80233058
[6,5175,33795] RAMDISK_READ inode:0 block:1 dst:0x80231020
[6,5176,33795] RAMDISK_READ inode:0 block:2 dst:0x80233058
[6,5177,33795] SIMPLE_READ inode:515 block:2 dst:0x80234858
[6,5178,33795] RAMDISK_READ inode:0 block:515 dst:0x80231020
[6,5179,33795] RAMDISK_READ inode:0 block:518 dst:0x80234858
[6,5180,33795] SIMPLE_READ inode:515 block:3 dst:0x80234858
[6,5181,33795] RAMDISK_READ inode:0 block:515 dst:0x80231020
[6,5182,33795] RAMDISK_READ inode:0 block:519 dst:0x80234858
[6,5183,33795] VM_MAP self:0x80538000 base:0x80539000 va:0x70000000 frame:0x8053b000
```
This is where the `init` process is started.  After a page fault
at address 0x70000000 (the initial program counter), the kernel allocates
a new frame and reads from the executable file (2).  It first looks
up the inode number for file 2 in the "stat inode" (inode 1).
It then finds the inode number for the executable (515).  It then
reads the inode and two of its blocks.  The reason it needs to read
two blocks is because a page is 4K while a block is only 2K.  Finally,
the frame is mapped.

## 💬 Ask Your LLM

- How does storing stat_entry records in inode 1 create a simple directory?
- Why does the flat layer treat inodes as arrays of blocks but files as arrays of bytes?
- How does the flat layer separate data representation from metadata management?
- Why does EmbryOS number files instead of naming them at this stage?

## 🧪 Exercise
Extend the flat layer with a function `flat_copy(from, to)` that
copies one file into another. Verify that the resulting file has
identical contents and metadata. Then modify `files_init()` to duplicate
an existing executable under a new file number and confirm that it
can still be run.
