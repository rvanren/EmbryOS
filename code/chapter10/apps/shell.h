#pragma once

#define N_RECTS 4
#define N_APPS  3

struct rect { const char *name; char x, y, w, h; };

void printf(struct screen *, char *);
void exec(struct screen *, char *);
