#pragma once

struct config {
    char *uart_type;
    uintptr_t uart_base;
    uintptr_t mem_end;
    uint64_t time_base;
};

extern struct config config;
