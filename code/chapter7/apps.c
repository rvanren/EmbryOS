#include <stddef.h>
#include <stdint.h>
#include "frame.h"
#include "sched.h"
#include "stdio.h"

__attribute__((noreturn)) void enter_user(void (*entry)());

void codeA(void){
    for (int cnt = 0;; cnt++) {
        user_put(2 + cnt % 3, 2 + cnt % 3, 'A' + cnt % 26, cnt, 7 - cnt % 8);
        for (volatile int i = 0; i < 100000; i++) ;
    }
}

void taskA(void) {
    enter_user(codeA);
}

void codeB(void) {
    user_put(10, 3, '$', 2, 0);
    for (int counter = 0;; counter++) {
        char c = user_get();
        user_put(10, 5 + counter % 10, c, 2, 0);
        if (c == '!') user_spawn(0, 40, 12, 40, 12);
        if (c == '.') user_exit();
    }
}

void taskB(void) {
    enter_user(codeB);
}

void (*applications[])() = { taskA, taskB };
int n_applications = sizeof(applications) / sizeof(applications[0]);
