#include <stdarg.h>

typedef struct {
    enum { STDIO_DEV, STDIO_FILE, STDIO_STRING } type;
    unsigned int offset;
    int eof;
    union {
        int file;
        char *string;
    } u;
} FILE;

extern FILE *stdin, *stdout, *stderr;

void putchar(char c);
void printf(const char *fmt, ...);
int fputc(int c, FILE *file);
void vfprintf(FILE *file, const char *fmt, va_list ap);
void fprintf(FILE *file, const char *fmt, ...);
int fputs(const char *s, FILE *file);
char *fgets(char *str, int size, FILE *file);
void sprintf(char *buf, const char *fmt, ...);
int readline(char *line, int size);
FILE *fopen(const char *name, const char *access);
void clearerr(FILE *file);
void fflush(FILE *file);
void fclose(FILE *file);
