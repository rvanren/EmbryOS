#include "string.h"

int strcmp(const char *p, const char *q) {
    while (*p != 0 && *q != 0 && *p == *q) { p++; q++; }
    if (*p == *q) return 0;
    if (*p == 0)  return -1;
    if (*q == 0)  return 1;
    return *p - *q;
}

char *strcpy(char *dst, const char *src) {
    char *ret = dst;
    while ((*dst++ = *src++) != '\0') ;
    return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = '\0';
    return dst;
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (size_t) (p - s);
}

char *strstr(const char *haystack, const char *needle) {
    if (*needle == 0) return (char *) haystack;
    for (const char *h = haystack; *h; h++) {
        const char *h2 = h, *n2 = needle;
        while (*h2 && *n2 && *h2 == *n2) { h2++; n2++; }
        if (!*n2) return (char *) h;
    }
    return 0;
}

char *strrchr(const char *s, int c) {
    const char *last = 0;
    char ch;
    while ((ch = *s++) != 0)
        if (ch == (char) c) last = s - 1;
    return (char *)((c == 0) ? s - 1 : last);
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && s[len] != '\0') len++;
    return len;
}
