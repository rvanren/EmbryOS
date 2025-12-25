#include "string.h"

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    for (; n--; p1++, p2++)
        if (*p1 != *p2) return *p1 - *p2;
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = s;
    unsigned char uc = (unsigned char) c;
    for (; n--; p++)
        if (*p == uc) return (void *) p;
    return 0;
}

void *memmove(void *dst, const void *src, size_t n) {
    unsigned char *d = dst;
    const unsigned char *s = src;
    if (d == s || n == 0) return dst;
    if (d < s)
        while (n--) *d++ = *s++;
    else {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    return memmove(dst, src, n);
}

void *memset(void *dst, int c, size_t n) {
    unsigned char *d = dst;
    while (n--) *d++ = c;
    return dst;
}
