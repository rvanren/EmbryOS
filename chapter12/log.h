#pragma once

#include <stdint.h>
#include "log_defs.h"

extern struct log_entry log[], __log_end[];
extern struct log_header log_header;

static inline struct log_entry *L(unsigned int level, enum log_type type, int arity) {
    struct log_header *lh = &log_header;
    uint32_t offset = 0;
    for (unsigned i = 0; i < level; i++) offset += lh->sizes[i];
    struct log_entry *le = &log[offset + lh->ts[level] % lh->sizes[level]];
    le->type = type;
    le->arity = arity;
    for (unsigned i = 0; i < LOG_N_LEVELS; i++) le->ts[i] = lh->ts[i];
    uintptr_t tmp;
    asm volatile ("mv %0, tp" : "=r"(tmp));
    le->self = (uint64_t)tmp;
    lh->ts[level]++;
    return le;
}

static inline void L0(unsigned int level, enum log_type type) {
    (void) L(level, type, 0);
}

static inline void L1(unsigned int level, enum log_type type,
        uint64_t w0) {
    struct log_entry *le = L(level, type, 1);
    le->payload[0] = w0;
}

static inline void L2(unsigned int level, enum log_type type,
        uint64_t w0, uint64_t w1) {
    struct log_entry *le = L(level, type, 2);
    le->payload[0] = w0;
    le->payload[1] = w1;
}

static inline void L3(unsigned int level, enum log_type type,
        uint64_t w0, uint64_t w1, uint64_t w2) {
    struct log_entry *le = L(level, type, 3);
    le->payload[0] = w0;
    le->payload[1] = w1;
    le->payload[2] = w2;
}

static inline void L4(unsigned int level, enum log_type type,
        uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3) {
    struct log_entry *le = L(level, type, 4);
    le->payload[0] = w0;
    le->payload[1] = w1;
    le->payload[2] = w2;
    le->payload[3] = w3;
}

static inline void L5(unsigned int level, enum log_type type,
        uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3, uint64_t w4) {
    struct log_entry *le = L(level, type, 5);
    le->payload[0] = w0;
    le->payload[1] = w1;
    le->payload[2] = w2;
    le->payload[3] = w3;
    le->payload[4] = w4;
}

void log_init(void);
