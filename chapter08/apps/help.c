#include "syslib.h"
#include "stdio.h"
#include "string.h"

char help[] =
//   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    "* There are 4 windows: ul ur ll lr.\n"
    "* The line syntax is [ul|ur|ll|lr] cmd\n"
    "* This runs cmd in the given window.\n"
    "* The default window is ur.\n"
    "* Change default by just [ul|ur|ll|lr]\n"
    "* Exit by typing exit or quit.\n"
    "* Change window focus using TAB.\n"
    "* Run 'help cmd' for help on cmd.";

char help_help[] = "Usage: help [cmd]\n";

char life_help[] = "Game of Life demo";

char snake_help[] =
    "Snake Game: use arrows to move snake.\n"
    "Type q to quit.";

void main(int argc, char **argv) {
    if (argc == 1) printf("%s", help);
    else if (strcmp(argv[1], "help") == 0) printf("%s", help_help);
    else if (strcmp(argv[1], "life") == 0) printf("%s", life_help);
    else if (strcmp(argv[1], "shell") == 0) printf("%s", help);
    else if (strcmp(argv[1], "snake") == 0) printf("%s", snake_help);
    else printf("Don't know about '%s'\n", argv[1]);
}
