// This function, written in assembly, saves the registers of the caller
// on the stack and then saves the stack pointer of the caller in *old_sp.
// Next, it replaces the sp with the value of new_sp, and restores the
// registers before returning.
extern void ctx_switch(void **old_sp, void *new_sp);

// Like ctx_switch, but the new stack does not contain any saved
// registers.  Instead, it calls the function exec_user().
extern void ctx_start(void **save_sp, void *new_sp);
