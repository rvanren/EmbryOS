#include "string.h"

int strcmp(const char *p, const char *q) {
    while (*p != 0 && *q != 0 && *p == *q) { p++; q++; }
    if (*p == *q) { return 0; }
    if (*p == 0) { return -1; }
    if (*q == 0) { return 1; }
    return *p - *q;
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++)
        dst[i] = src[i];
    for (; i < n; i++)
        dst[i] = '\0';
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    unsigned char *d = dst;
    while (n--) *d++ = c;
    return dst;
}
