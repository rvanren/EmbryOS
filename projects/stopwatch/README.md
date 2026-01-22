# Project: Stopwatch

## Overview

In this project, you will build a **graphical stopwatch application** that runs as a user-level program on EmbryOS. The stopwatch responds to keyboard input, tracks elapsed time with nanosecond precision, and renders its output using the **blockpixel** graphics library.

This project has two main components:

1. **A new system call** to obtain the current time.
2. **A user-space stopwatch application** with interactive controls and a graphical display.

The emphasis is on **correct system interaction**, **event-driven programming**, and **clear visual feedback**.

---

## Part 1: `user_gettime()` System Call

### Goal

Implement a user-level system call:

```c
uint64_t user_gettime(void);
```

This function must return the **number of nanoseconds elapsed since system boot**.

### Kernel Support

The kernel already provides:

```c
uint64_t mtime_get(void);   // returns ticks since boot
uint64_t time_base;        // ticks per second
```

### Requirements

* Convert ticks to **nanoseconds**.
* Should work both on 32-bit and 64-bit RISC-V architecture
    (see, for example, the implementation of `mtime_get` in sbi.S)

### Expected Behavior

* `user_gettime()` returns `0` shortly after boot.
* Time increases continuously while the system is running.
* The function must be callable from user space without direct access to kernel symbols.

---

## Part 2: Stopwatch Application

### Program Behavior

The stopwatch maintains two independent pieces of state:

* **Running state**: running or stopped
* **Elapsed time**: accumulated stopwatch time

The initial state is:

* **Stopped**
* **Elapsed time = 0**

---

### Input Handling

All input is received via:

```c
int user_get(int block);
```

If `block` is non-zero, then the process will block until an input
event is available.
Your program must handle the following input events:

#### Keyboard Events

* **`'s'`**
  Toggle the stopwatch:

  * If stopped → start running
  * If running → stop

* **`'r'`**
  Reset elapsed time to **0**
  (running/stopped state must remain unchanged)

* **`'q'`**
  Terminate process

#### Input Focus Events

* `USER_GET_GOT_FOCUS`
* `USER_GET_LOST_FOCUS`

These events indicate whether the application currently has user focus.
Users can change the input focus using `TAB`.
Your UI must make the current focus state **visually clear**.

#### Other Events

* `USER_GET_NO_INPUT`

Returned if `block = 0` and no input event is available.

---

### Timing Semantics

* While running, elapsed time advances using `user_gettime()`.
* While stopped, elapsed time remains constant.
* Timekeeping must be resilient to pauses, redraws, and focus changes.

A typical approach is to track:

* `start_time` (from `user_gettime()`)
* `accumulated_time`

---

## Display and Graphics

### Blockpixel Library

* Resolution: **39 × 22 block pixels**
* Each block pixel supports **8 colors**

You must use the blockpixel API for all rendering.

---

### Stopwatch Display Requirements

* Display must show **at least**:

  * Minutes (0–59)
  * Seconds (0–59)

* You may optionally include:

  * Tenths of seconds
  * Hundredths of seconds

* The design of the stopwatch face is **entirely up to you**:

  * Digital or analog
  * Large digits or compact layout
  * Color usage to indicate state (running, stopped, focus)

---

### State Visibility

At all times, the user must be able to clearly see:

* Whether the stopwatch is **running or stopped**
* Whether the application **has focus**
* The current elapsed time

---

## Constraints and Expectations

* Event-driven input handling
* When stopped, the stopwatch should not use any CPU time
* Clean separation between:

  * Timekeeping logic
  * Input handling
  * Rendering
* Code must be readable and well-commented
* Use of AI should be clearly documented.  Note that it is allowed to use
  these tools, but we would like to understand how you use it.

---

## Deliverables

Implement your code within `chapter12`.  Submit a tar file of this
directory, which you can create by running the following command in
the `chapter12` directory: `make clean; tar cf ../stopwatch.tar .`
Also submit `log/log_events.def` (we encourage you to add new log
events).

---

## Summary

This project exercises:

* System call design
* Time conversion and precision
* Event-driven user interfaces
* Graphical output under tight constraints

Correctness, clarity, and robustness matter more than visual complexity.
