#pragma once

#define SELFIE_MAX_FILES    8
#define SELFIE_NROWS       11
#define SELFIE_NCOLS       39

struct selfie_file {
    int file;       // flat file number
    int pos;
};

struct selfie {
    int initialized, col;
    uword_t brk;
    struct selfie_file files[SELFIE_MAX_FILES];
    char window[SELFIE_NROWS][SELFIE_NCOLS];
};

void selfie_syscall_handler(struct trap_frame *tf);
