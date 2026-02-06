# Sleeping

## Overview

In this project, you will add **proper sleep support** to EmbryOS by implementing a new system call:

```c
void user_sleep(uint64_t deadline);
```

This system call suspends the calling process until a specified **absolute time**, expressed in **nanoseconds since boot**, has been reached.

Unlike earlier delay mechanisms, this project explicitly **eliminates busy waiting** in user space. Instead, sleeping processes must be descheduled by the kernel and resumed only when their deadline expires.

This project requires **kernel modifications** to the scheduler as well as updates to existing user-space timing utilities.

---

## Goals

By completing this project, you will:

* Extend the EmbryOS system call interface
* Modify the kernel scheduler to support timed sleep
* Integrate sleeping with the system timer
* Replace user-level busy waiting with proper blocking
* Gain experience with time-based process scheduling

---

## Part 1: `user_sleep(uint64_t deadline)`

### System Call Semantics

`user_sleep()` suspends the calling process until:

```
user_gettime() >= deadline
```

Where:

* `deadline` is an **absolute time**, not a duration
* Time is measured in **nanoseconds since boot**

### Requirements

* The calling process must **not consume CPU time** while sleeping
* The process must resume execution soon after the deadline expires
* The system call must be safe to invoke repeatedly and from any user process
* A deadline in the past should return immediately

---

## Part 2: Scheduler Modifications (`sched.c`)

This project requires changes to the EmbryOS scheduler.

### Sleeping Processes

You will need to introduce a way to represent processes that are:

* Runnable
* Sleeping (blocked until a specific deadline)
* Otherwise inactive

Each sleeping process must be associated with a **wake-up time**.
(Figure out where per-process information is kept and add the necessary
 fields.)

---

### Scheduler Responsibilities

The scheduler must:

1. **Exclude sleeping processes** from normal run queues
2. Periodically check whether a sleeping process’s deadline has expired
3. Move expired processes back into the runnable state
4. Ensure that sleeping processes do not starve runnable processes

You may choose the data structure and policy used to track sleeping processes, provided the behavior is correct and efficient.

---

### Timing Source

The kernel already provides:

```c
uint64_t mtime_get(void);
uint64_t time_base;
```

You should use the same timebase as `user_gettime()` to ensure consistent semantics across the system.

---

## Part 3: Updating `user_delay()`

### Current Behavior

`user_delay()` currently uses **busy waiting**, repeatedly checking the current time until a delay expires.

This is no longer acceptable.

---

### Required Change

You must rewrite `user_delay()` so that it:

* Computes an absolute deadline
* Calls `user_sleep(deadline)`
* Does **not** spin or poll

After this change:

* Delays should not waste CPU cycles
* Multiple user processes should be able to sleep concurrently without interference

---

## Correctness Requirements

Your implementation must satisfy the following:

* Sleeping processes do not run until their deadline (i.e.,
  a subsequent call to `user_gettime()` is guaranteed to return a number
  larger than `deadline`)

---

## Testing Expectations

You should test your implementation by:

* Sleeping for short and long durations
* Running multiple sleeping processes concurrently
* Verifying that CPU usage drops when processes are sleeping
  (note that the `sched_idle` loop uses the RISC-V `wfi` instruction
   to put the hart to sleep until the next interrupt)
* Confirming that existing applications (e.g., splash and life) still function correctly

---

## Deliverables

Implement your code within `chapter12`.  Submit a tar file of this
directory, which you can create by running the following command in
the `chapter12` directory: `make clean; tar cf ../sleep.tar .`
Also submit `log/log_events.def` (we encourage you to add new log
events).

In `chapter12`, include a file called "explanation.md" which explains:

* How sleeping processes are represented
* How wake-ups are triggered
* Any scheduler design decisions you made
* How you used AI, if at all

---

## Constraints

* No busy waiting in user space
* No busy waiting in the scheduler
* Clean, readable code
* Minimal impact on unrelated kernel subsystems

---

## Summary

This project turns EmbryOS from a "spin-based" system into a **properly blocking OS**.

It is a foundational step toward:

* Efficient multitasking
* Scalable concurrency
* Realistic operating-system behavior

Take care with time units, scheduler invariants, and edge cases.
