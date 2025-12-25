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
    "* Run 'cat README' for more info.\n"
    "* Run 'help cmd' for help on cmd.";

char cat_help[] =
    "Usage: cat file1 file2 ...\n"
    "Outputs the contents in the files.";

char help_help[] = "Usage: help [cmd]\n";

char life_help[] = "Game of Life demo";

char ls_help[] = "List file names and sizes";

char selfie_help[] = "The Selfie C* Compiler";

char snake_help[] =
    "Snake Game: use arrows to move snake.\n"
    "Type q to quit.";

void main(int argc, char **argv) {
    if (argc == 1) printf("%s", help);
    else if (strcmp(argv[1], "cat") == 0) printf("%s", cat_help);
    else if (strcmp(argv[1], "help") == 0) printf("%s", help_help);
    else if (strcmp(argv[1], "life") == 0) printf("%s", life_help);
    else if (strcmp(argv[1], "ls") == 0) printf("%s", ls_help);
    else if (strcmp(argv[1], "selfie") == 0) printf("%s", selfie_help);
    else if (strcmp(argv[1], "shell") == 0) printf("%s", help);
    else if (strcmp(argv[1], "snake") == 0) printf("%s", snake_help);
    else printf("Don't know about '%s'\n", argv[1]);
}
