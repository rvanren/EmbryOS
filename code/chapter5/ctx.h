extern void ctx_switch(void **old_sp, void *new_sp);
extern void ctx_start(void **save_sp, void *new_sp, void (*entry)(void));
extern void ctx_user(void **save_sp, void *new_sp, void (*entry)(void));
