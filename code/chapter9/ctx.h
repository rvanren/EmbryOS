// This function, written in assembly, saves the registers of the caller
// on the stack and then saves the stack pointer of the caller in *old_sp.
// Next, it replaces the sp with the value of new_sp, and restores the
// registers before returning.
extern void ctx_switch(void **old_sp, void *new_sp);

// Like ctx_switch, but the new stack does not contain any saved
// registers.  Instead, it calls the function entry().
extern void ctx_start(void **save_sp, void *new_sp, void (*entry)(void));

// Like ctx_start, but calls entry() in user mode.
extern void ctx_user(void **save_sp, void *new_sp, void (*entry)(void));

// This function sets the PMP registers to allow access to kernel code
// in user space.  It is a temporary hack that should be removed when
// virtual memory is implemented.
extern void ctx_user_setup(void);
