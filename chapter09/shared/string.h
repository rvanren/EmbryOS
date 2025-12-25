#include <stddef.h>

int strcmp(const char *p, const char *q);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
size_t strlen(const char *s);
char *strstr(const char *haystack, const char *needle);
char *strrchr(const char *, int);
size_t strnlen(const char *, size_t);

void *memmove(void *dst, const void *src, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memchr(const void *, int, size_t);
