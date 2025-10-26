#include "syslib.h"
#include "stdio.h"

char help[] =
//   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    "* There are 4 windows: ul ur ll lr.\n"
    "* The line syntax is [ul|ur|ll|lr] cmd\n"
    "* This runs cmd in the given window.\n"
    "* The default window is ur.\n"
    "* Change default by just [ul|ur|ll|lr]\n"
    "* Exit by typing exit or quit.\n"
    "* Change window focus using TAB";

void main(void) {
    printf("%s", help);
}
