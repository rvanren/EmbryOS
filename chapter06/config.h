#pragma once

struct config {
    char *uart_type;
    uintptr_t uart_base;
    uintptr_t mem_end;
};

extern struct config config;
