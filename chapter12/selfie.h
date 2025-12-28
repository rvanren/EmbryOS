#pragma once

#define SELFIE_MIN_FILE     3	// minimum file descriptor
#define SELFIE_MAX_FILE     6	// maximum file descriptor + 1
#define SELFIE_NROWS       11
#define SELFIE_NCOLS       39
#define SELFIE_BUF         64

struct selfie_file {
    int pos;            // position in the file
    int16_t file;       // flat file number
    int16_t buf_size;   // #characters buffered
    int16_t buf_pos;    // position in the buffer
    char buf[SELFIE_BUF];
};

struct selfie {
    int16_t initialized, col;
    uintptr_t brk;
    struct selfie_file files[SELFIE_MAX_FILE - SELFIE_MIN_FILE];
    char window[SELFIE_NROWS][SELFIE_NCOLS];
};

void selfie_syscall_handler(struct trap_frame *tf);
