#include <stdint.h>
#include "stdio.h"
#include "process.h"
#include "screen.h"
#include "ctx.h"

#define PAGE_SIZE      4096
#define MAX_PROCESSES     8

union proc_page {
    struct pcb pcb;
    char page[PAGE_SIZE];
};

static union proc_page procs[MAX_PROCESSES];
static int proc_current, proc_count;

void proc_put(struct pcb *p, int row, int col, char ch, int fg, int bg) {
    if (row < 0 || row >= p->area.h) return;
    if (col < 0 || col >= p->area.w) return;
    screen_move(p->area.y + row, p->area.x + col);
    screen_put(ch, fg, bg);
}

void proc_init(){
    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 0);
    int pid = proc_count++;
    union proc_page *p = &procs[pid];
    proc_current = pid;
}

void proc_create(entry_t fn, int x, int y, int w, int h) {
    if (proc_count >= MAX_PROCESSES) {
    printf("NO MORE PROCESSES\n");
        return;
    }
    int pid = proc_count++, prev = proc_current;
    union proc_page *p = &procs[pid];
    p->pcb.area = (struct rect){ .x = x, .y = y, .w = w, .h = h };
    proc_current = pid;
    ctx_start(&procs[prev].pcb.sp, p->page + PAGE_SIZE, fn);
}

void proc_yield(void) {
    int prev = proc_current;
    proc_current = (proc_current + 1) % proc_count;
    ctx_switch(&procs[prev].pcb.sp, procs[proc_current].pcb.sp);
}

struct pcb *proc_self(void) {
    return &procs[proc_current].pcb;
}
