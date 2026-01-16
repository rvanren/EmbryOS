# Chapter 07: Terminal Emulation and the Shell

Many simple applications are text-based and do not require a fancy GUI.
For this, the classic terminal interface is ideal.  The shell is a
text-based interface to start new applications.

## Learning Objectives

- Build a simple user–space library for text output and input editing.
- Implement scrolling text windows that improve readability and layout.
- Introduce readline(), which performs line editing with echo and delete handling.
- Learn how to construct a basic shell that reads commands and launches applications.
- Observe how user–level abstractions can be layered without kernel changes.

## New or Modified Modules

| File | Purpose |
|------|---------|
| `apps/window.h` | Defines the window abstraction: a 2-D array of cells with current cursor and scrolling behavior.|
| `apps/window1.c` | Window output |
| `apps/window2.c` | Window input |
| `apps/putchar.c` | Defines `putchar()` and `readline()`. Each process owns a single window. |
| `apps/stdio.h`, `apps/printf.c` | Formatted output through `putchar()` |
| `apps/shell.c` | Implements the shell |
| `apps/help.c` | Provides help to users |
| `apps.c` | Passing arguments to user processes |

## Key Functions & Symbols

| Name | Description |
|------|-------------|
| `struct window window` | Current state of a terminal window |
| `putchar(c)` | Sends a character to the terminal window |
| `readline(line, size)` | Reads a line of input with basic editing |
| `window_init()` | Initializes a terminal window |
| `window_sync()` | Copies window state to the screen |
| `window_scroll()` | Scrolls the window abstraction |
| `window_putchar()` | Adds a character to the window abstraction |
| `window_readline()` | Reads a line |
| `exec_user()` | Prepares arguments on the stack of a new process |

## Discussion

This chapter adds a lightweight text interface entirely in user
space.  Each application manages a window: a rectangular region
that scrolls automatically as text is printed. The window layer
wraps system calls such as `user_put()` and `user_get()` into
convenient functions that implement scrolling, cursor motion, and
line editing.  Function `readline()` reads one line of text
from the keyboard. It echoes each character as it is typed, supports
backspace and delete, and terminates on newline.

The shell program uses these capabilities to form an interactive
command interpreter.  The shell supports multiple display regions,
assigning each process its own window for output.  It displays a
prompt, reads a line using `readline()`, tokenizes it into arguments,
and calls `user_spawn()` to run the selected application.

Inside the kernel, the `exec_user()` prepares the argument vector
that is passed to applications, which is done in a way that is
compatible with UNIX systems such as Linux.  In particular, when a
process starts executing, its stack pointer points to the argument
count followed by a vector of pointers to each argument.

## Check the Log

Try some experiments and see what you can find in the log.
For example, I typed "snake" in the shell and then "ll" in
the snake app.  I was able to find this in the log:

```
% ./logdump qemu.elf mem.bin | grep IO_RECEIVED
[8,31,75316] IO_RECEIVED focus:0x80529000 c:'s'
[8,35,75349] IO_RECEIVED focus:0x80529000 c:'n'
[8,39,75387] IO_RECEIVED focus:0x80529000 c:'a'
[8,43,75425] IO_RECEIVED focus:0x80529000 c:'k'
[8,47,75462] IO_RECEIVED focus:0x80529000 c:'e'
[8,51,75529] IO_RECEIVED focus:0x80529000 c:<RETURN>
[8,65,78396] IO_RECEIVED focus:0x8052e000 c:'l'
[8,69,78446] IO_RECEIVED focus:0x8052e000 c:'l'
```

## 💬 Ask Your LLM

- How does the window abstraction prevent one program’s output from overwriting another’s region of the screen?
- Why is it better to implement line editing in user space rather than in the kernel?
- How does readline() use `user_get()` and `user_put()` internally?
- What advantages do scrolling windows provide for building richer user interfaces?
- How are arguments passed from the shell to applications via user_spawn()?

## Exercise

- Modify window_readline() so that it supports left–arrow and right–arrow cursor motion
in addition to backspace. Test it by adding a simple history feature to the shell that remembers the last line entered.
