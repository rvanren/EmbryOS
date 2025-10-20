#include <stdint.h>

void apps_init();

__attribute__((noreturn))
void enter_user(void *entry, uintptr_t gp_val, uintptr_t user_sp, uintptr_t ksp);
