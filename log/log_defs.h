#pragma once

#include <stdint.h>

#define LOG_N_LEVELS    3
#define LOG_MAX_PAYLOAD 5

// Some defined levels
#define L_BASE 0
#define L_NORM 1
#define L_FREQ 2

enum log_arg_kind {
    ARG_CHAR,
    ARG_INT,
    ARG_UINT,
    ARG_PTR,
    ARG_CELL,
    ARG_CAUSE,
};

enum log_type {
#define LOG_EVENT(name, arity, ...) L_##name,
#include "log_events.def"
#undef LOG_EVENT
    LOG_TYPE_MAX
};

struct log_entry {      // 64 bytes
    uint16_t type;
    uint8_t  arity;
    uint8_t  reserved;
    uint32_t ts[LOG_N_LEVELS];      // timestamp
    uint64_t self;                  // tp register
    uint64_t payload[LOG_MAX_PAYLOAD];
};

struct log_header {
    uint32_t ts[LOG_N_LEVELS];      // timestamp
    uint32_t sizes[LOG_N_LEVELS];
};
