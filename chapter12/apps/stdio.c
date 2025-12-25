#include "types.h"
#include "syslib.h"
#include "stdlib.h"
#include "stdio.h"
#include "dir.h"
#include "malloc.h"

static FILE dev = { .type = STDIO_DEV };
FILE *stdin = &dev, *stdout = &dev, *stderr = &dev;

int fputc(int c, FILE *file) {
    char ch = c;
    switch (file->type) {
    case STDIO_DEV:    putchar(ch); break;
    case STDIO_FILE:   user_write(file->u.file, file->offset, &ch, 1); break;
    case STDIO_STRING: file->u.string[file->offset] = ch; break;
    }
    file->offset++;
}

int fputs(const char *s, FILE *file) {
    while (*s != 0) fputc(*s++, file);
}

static void print_unsigned(FILE *file, uword_t x, unsigned int base) {
    char buf[32];
    int i = 0;
    do {
        int d = x % base;
        buf[i++] = (d < 10) ? '0'+d : 'a'+(d-10);
        x /= base;
    } while (x);
    while (--i >= 0) fputc(buf[i], file);
}

void vfprintf(FILE *file, const char *fmt, va_list ap) {
    for (; *fmt; fmt++) {
        if (*fmt != '%') { fputc(*fmt, file); continue; }
        fmt++;
        switch (*fmt) {
        case 'd': {
            int x = va_arg(ap, int);
            if (x < 0) { fputc('-', file); x = -x; }
            print_unsigned(file, (uword_t) x, 10);
            break;
        }
        case 'D': {
            sword_t x = va_arg(ap, sword_t);
            if (x < 0) { fputc('-', file); x = -x; }
            print_unsigned(file, (uword_t) x, 10);
            break;
        }
        case 'u': print_unsigned(file, va_arg(ap, unsigned int), 10); break;
        case 'U': print_unsigned(file, va_arg(ap, uword_t),      10); break;
        case 'x': print_unsigned(file, va_arg(ap, unsigned int), 16); break;
        case 'X': print_unsigned(file, va_arg(ap, uword_t),      16); break;
        case 'p': print_unsigned(file, va_arg(ap, uintptr_t),    16); break;
        case 'c': fputc(va_arg(ap, int), file); break;
        case 's': {
            char *s = va_arg(ap, char*);
            while (*s) fputc(*s++, file);
            break;
        }
        case '%': fputc('%', file); break;
        default:  fputc('%', file); fputc(*fmt, file);
        }
    }
}

void fprintf(FILE *file, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(file, fmt, ap);
    va_end(ap);
}

void sprintf(char *s, const char *fmt, ...) {
    FILE file;
    file.type = STDIO_STRING;
    file.u.string = s;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(&file, fmt, ap);
    va_end(ap);
    s[file.offset] = 0;
}

void printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

FILE *fopen(const char *name, const char *access) {
    if (*access == 'r') {
        int fh = dir_lookup(name);
        if (fh < 0) return 0;
        FILE *file = calloc(1, sizeof(*file));
        if (file == 0) return 0;
        file->type = STDIO_FILE;
        file->u.file = fh;
        return file;
    }
    if (*access == 'w') {
        int fh = user_create();     // create a new file
        if (fh < 0) return 0;
        dir_delete(name);           // delete the old file if any
        dir_create(name, fh);
        FILE *file = calloc(1, sizeof(*file));
        if (file == 0) return 0;
        file->type = STDIO_FILE;
        file->u.file = fh;
        return file;
    }
    return 0;
}

char *freadline(char *str, int size, FILE *file) {
    if (file->eof) return 0;
    int nread = 0;
    while (nread < size - 1) {
        int n = user_read(file->u.file, file->offset, &str[nread], 1);
        if (n == 0) {
            file->eof = 1;
            break;
        }
        file->offset++;
        nread++;
        if (str[nread - 1] == '\n') break;
    }
    str[nread] = 0;
    return str;
}

char *fgets(char *str, int size, FILE *file) {
    switch (file->type) {
    case STDIO_DEV:
        return readline(str, size) ? str : 0;
    case STDIO_FILE:
        return freadline(str, size, file);
    default:
        return 0;
    }
}

int feof(FILE *file) { return file->eof; }

void fflush(FILE *file) { /* no buffering */ }

void clearerr(FILE *file) { file->eof = 0; }

void fclose(FILE *file) {
    if (file->type == STDIO_FILE) free(file);
}
