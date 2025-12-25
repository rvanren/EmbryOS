// This is an "exception handler" function that can be called
// with a descriptive message.  When called within the context
// of a process, it kills the process.  When called within the
// context of the kernel, it "panics" (halts the kernel).
void die(void *msg);
