# User-Level Threads, Blocking I/O, and Synchronization

## Overview

In this project, you will implement a **user-level threading library** that supports:

* Cooperative multithreading
* Blocking sleep
* Blocking input
* Counting semaphores

All threads run within a **single EmbryOS user process**. The kernel is unaware of threads and schedules only the process as a whole. All thread scheduling, blocking, and synchronization happen entirely in **user space**.

In addition to the library itself, you will build an **animated application** that demonstrates concurrency and blocking input without freezing animation.

---

## Threading Model

* Threads are **cooperatively scheduled**
* There is **no preemption**
* A thread runs until it:

  * calls `thread_yield()`
  * calls `thread_sleep()`
  * blocks in `thread_get()`
  * blocks in `sema_dec()`
  * exits (i.e., calls `thread_exit()` or returns from its main function)
* Context switches are explicit and use provided assembly helpers

---

## Required API

You must implement the following interface:

```c
void thread_init();
void thread_create(void (*f)(void *), void *arg, unsigned int stack_size);
void thread_yield();
void thread_sleep(uint64_t ns);
int  thread_get();
void thread_exit();

struct sema *sema_create(unsigned int count);
void sema_inc(struct sema *sema);
void sema_dec(struct sema *sema);
void sema_release(struct sema *sema);
```

---

## Program Structure Requirements

* `main()` **must call** `thread_init()` first, become the first thread
* `main()` **must call** `thread_exit()` last, but this should not terminate the program if other threads are still running
* Any other thread whose function returns must **automatically call**
  `thread_exit()` (so all but the first thread do not have to call
  `thread_exit` explicitly)
* `thread_exit()` should never return
* When the last thread exits, or when all remaining threads are blocked
  on semaphore, the program should terminate.
* As long as there are threads running or waiting for input, the program
  should not terminate.

---

## Context Switching Primitives

You must use the following functions from `ctx.h`:

```c
extern void ctx_switch(void **old_sp, void *new_sp);
extern void ctx_start(void **save_sp, void *new_sp);
```

### Semantics

* **`ctx_switch()`**

  * Saves the caller’s registers on its stack
  * Stores the caller’s stack pointer in `*old_sp`
  * Switches to `new_sp`
  * Restores registers and resumes execution

* **`ctx_start()`**

  * Used to start a brand-new thread
  * The new stack contains no saved registers
  * Begins execution by calling `exec_user()`

---

## Thread Lifecycle

### Thread Creation

```c
thread_create(f, arg, stack_size);
```

* Calls `f(arg)` as a thread
* Allocates a stack of at least `stack_size` bytes, aligned at 16 bytes)
    (using `malloc`, which already aligns at 16 bytes)

---

### Thread Termination

```c
void thread_exit();
```

* Removes the calling thread from scheduling
* Eventually frees associated resources (stack, control block)
* Switches to another runnable thread
* Recall that a thread cannot clean itself up (with no stack, the code would not be able to call `ctx_switch`)
* A thread that returns from its main function should call this function implicitly

---

## Blocking Sleep

```c
void thread_sleep(uint64_t deadline);
```

* Suspends the calling thread until deadline expires
  (where deadline is the time in nanoseconds since boot time)
* Must **not** busy wait

---

## Blocking Input: `thread_get()`

### Semantics

```c
int thread_get();
```

`thread_get()` behaves **exactly like `user_get(1)`**, but at the **thread level**.

It:

* **Blocks the calling thread**
* Returns one of:

  * A character
  * `USER_GET_GOT_FOCUS`
  * `USER_GET_LOST_FOCUS`

There is **no non-blocking variant**.

---

### Correct Behavior

* If input is already available, return immediately
* Otherwise:

  * Block the calling thread
  * Yield to another runnable thread
* When input arrives:

  * Wake **exactly one** thread waiting for input
  * Deliver the input event to that thread
* Input events must **not be lost**

## Semaphores

You must implement **counting semaphores**.

### Creation

```c
struct sema *sema_create(unsigned int count);
```

---

### Operations

```c
void sema_dec(struct sema *sema);   // P(rocure)
void sema_inc(struct sema *sema);   // V(acate)
void sema_release(struct sema *sema);
```

#### Semantics

* `sema_dec()`:

  * If count > 0, decrement and return
  * If count == 0, block the calling thread

* `sema_inc()`:

  * Wake **exactly one** blocked thread, if any
  * Otherwise increment the count

* `sema_release()`:

  * Frees the semaphore

---

## Scheduler Requirements

Your scheduler must support blocking for **four distinct reasons**:

1. Runnable
2. Sleeping (`thread_sleep`)
3. Waiting for input (`thread_get`)
4. Waiting on a semaphore (`sema_dec`)

Blocked threads must never appear in the runnable queue until their blocking condition is resolved.

---

## Animated Demo Application

You must build an animated application that:

* Creates multiple threads
* Uses `thread_sleep()` for timing
* Uses `thread_get()` for input
* Uses semaphores for synchronization
* Continues animating **while input is blocked**

Examples:

* Moving objects
* Bouncing shapes
* Progress bars
* Multiple independently animated elements

The demo must clearly show that:

* Blocking input does **not** freeze the application
* Threads resume correctly after input arrives

---

## Files

This project directory contains the following two files:

* `thread.h`: thread interface
* `t_pong.c`: multi-threaded version of the `pong` game as an example

## Deliverables

Implement your code within `chapter12/apps`.
There should be at least the following three files:

* `thread.h`: thread interface (which you can just copy from this directory)
* `thread.c`: thread interface implementation
* `game.c`: your game

Submit a tar file of this
directory, which you can create by running the following command in
the `chapter12/apps` directory: `make clean; tar cf ../threading.tar .`

In `chapter12/apps`, include a file called "explanation.md" which explains:

* Thread control block design
* Run-queue organization
* Input multiplexing approach
* Semaphore blocking and wake-up logic
* Instructions on how to play the game
* How you used AI, if at all

---

## Constraints

* No kernel modifications
* All context switching must use `ctx_switch()` / `ctx_start()`

---

## Summary

This project implements a **complete user-level runtime system**:

* Cooperative threads
* Blocking sleep
* Blocking I/O
* Synchronization primitives

Correctness depends on careful scheduling, clean separation of concerns, and precise handling of blocking conditions. This is one of the most conceptually important projects in EmbryOS.  Design it carefully before you code.
