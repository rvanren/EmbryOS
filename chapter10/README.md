# Chapter 10: User Space Files and Directories

Human users like to name files by character strings rather than by
number.  A directory maps string names to file numbers.  This chapter
also makes the flat file interface interface available to applications.
This ability completes a fully functioning operating system.

## Learning Objectives

- Introduce a simple directory abstraction that maps file names to file numbers.
- Learn how the directory is implemented as a user–level program layered on top of the flat file interface.
- Understand how user programs now load and execute other programs through directory lookups and the file interface.
- Observe how the flat storage model evolves into a primitive file system with a namespace.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `shared/dir.h` | Defines a directory entry and the directory interface |
| `files.c` | Initializes the root directory |
| `apps/syslib.[Sh]` | System call interface extended with file interface |
| `apps/dir.c` | Library that implements the directory interface |
| `apps/cat.c` | Application that prints files to output |
| `apps/ls.c` | Application that lists the files and their sizes |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `files_init()` | Initializes the root directory |
| `int user_create(void)` | Creates a new file and return its file number |
| `int user_read(file, offset, dst, n)` | Reads a part of a file |
| `int user_write(file, offset, src, int n)` | Writes a part of a file |
| `int user_size(file)` | Returns the size of a file |
| `void user_delete(file)` | Deletes a file |
| `int dir_create(name, file)` | Adds or updates entry name and maps to file |
| `void dir_delete(name)` | Deletes the given entry and the file it points to |
| `void dir_list(void (*fn)(name, file))` | Invokes `fn` for every mapping in the root directory |
| `int dir_lookup(name)` | Looks up name and returns its file identifier |

## Discussion

This chapter introduces the directory abstraction and extends the
user environment to give full access to files.
The directory is implemented in user space as a library
that manages one special file (file 1) used to store directory
entries. Each entry contains a fixed-length name and the numeric
identifier of a file managed by the flat layer.

At system initialization, `files_init()` initializes this directory
with the files embedded with the kernel. The current implementation
supports only a single directory.  Its contents can be listed using
the `ls` application.  Files can be printed to output using the
`cat application`.  The only printable file, at this time, is a
file called `README`.

## Check the Log

User processes are now able to read files.  Here are the system calls that
are in the log after running `cat README`:

```
[8,2710,112189] USER_SPAWN self:0x8053d000 app:10
[8,2751,112212] USER_SIZE file:1
[8,2756,112228] USER_READ file:1 offset:0 addr:0x3fff38 n:16
[8,2764,112230] USER_GET block:1
[8,2766,112232] USER_READ file:1 offset:16 addr:0x3fff38 n:16
[8,2774,112233] USER_READ file:1 offset:32 addr:0x3fff38 n:16
[8,2782,112234] USER_READ file:1 offset:48 addr:0x3fff38 n:16
[8,2790,112235] USER_READ file:1 offset:64 addr:0x3fff38 n:16
[8,2798,112236] USER_READ file:1 offset:80 addr:0x3fff38 n:16
[8,2806,112237] USER_READ file:1 offset:96 addr:0x3fff38 n:16
[8,2814,112238] USER_READ file:1 offset:112 addr:0x3fff38 n:16
[8,2822,112239] USER_READ file:1 offset:128 addr:0x3fff38 n:16
[8,2830,112240] USER_READ file:1 offset:144 addr:0x3fff38 n:16
[8,2838,112241] USER_READ file:11 offset:0 addr:0x3fff77 n:1
[8,2846,113629] USER_READ file:11 offset:1 addr:0x3fff77 n:1
[8,2854,113633] USER_READ file:11 offset:2 addr:0x3fff77 n:1
...
```

First, the function `dir_lookup("README")` requests the size of the
directory and then reads the 16-byte entries one by one until it finds
"README".  "README" is file 11 in this case.  Then, ``cat`` proceeds to
read the characters of "README" one at a time.
(The `USER_PUT` system calls that happened after every `USER_READ` in file 11 are
in the level 3 log and are no longer available.)

## 💬 Ask Your LLM

- Why is the directory implemented as a user-space library instead of a kernel module?
- How does the directory file relate to the flat layer introduced earlier?
- Why is it safe for the kernel to rely on numeric file identifiers while applications use names?”
- What would be required to support nested directories or pathnames?

## Exercise

Implement nested directories.
