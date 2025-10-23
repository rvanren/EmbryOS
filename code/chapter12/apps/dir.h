#pragma once

#define NAME_LEN 14
struct dirent { char name[NAME_LEN]; uint16_t file; };

int dir_lookup(const char *name);
int dir_create(const char *name, int file);
void dir_delete(const char *name);
void dir_list(void (*fn)(const char *name, int file));
