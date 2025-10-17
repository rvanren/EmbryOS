#include "screen.h"
#include "ctx.h"

#define PAGE_SIZE   4096
#define MAX_PROCESSES 8

struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

struct pcb {
    void *sp;           // saved stack pointer
    int pid;
    int state;          // 0=unused, 1=ready, 2=running
    struct rect area;   // process’s allowed screen region
};

typedef void (*entry_t)(void);

union process_page {
    struct pcb pcb;
    unsigned char page[PAGE_SIZE];
};

static union process_page procs[MAX_PROCESSES];
static int current = -1;
static int nprocs = 0;

/* Convert process-local coordinates to global and draw */
void process_put(struct pcb *p, int row, int col, char ch, int fg, int bg) {
    if (row < 0 || row >= p->area.h) return;
    if (col < 0 || col >= p->area.w) return;

    int global_row = p->area.y + row;
    int global_col = p->area.x + col;

    screen_move(global_row, global_col);
    screen_put(ch, fg, bg);
}

/* crude busy-wait delay */
static void delay(void) {
    for (volatile int i = 0; i < 300000; i++);
}

/* Create a new process running function fn inside a screen rectangle */
int create_process(entry_t fn, int x, int y, int w, int h) {
    if (nprocs >= MAX_PROCESSES) return -1;

    int pid = nprocs++;
    union process_page *p = &procs[pid];
    p->pcb.pid = pid;
    p->pcb.state = 1;
    p->pcb.area.x = x;
    p->pcb.area.y = y;
    p->pcb.area.w = w;
    p->pcb.area.h = h;

    void *stack_top = p->page + PAGE_SIZE;
    ctx_start(&p->pcb.sp, stack_top, fn);
    return pid;
}

/* Round-robin cooperative scheduler */
void schedule(void) {
    int next = (current + 1) % nprocs;
    int prev = current;
    current = next;

    if (prev == -1) {
        // first activation
        void *new_sp = procs[next].pcb.sp;
        ctx_switch(&procs[next].pcb.sp, new_sp);
    } else {
        ctx_switch(&procs[prev].pcb.sp, procs[next].pcb.sp);
    }
}

/* ------------------------------------------------------------
 * Example user processes
 * ------------------------------------------------------------ */
void taskA(void) {
    struct pcb *self = &procs[current];
    int color = 2;  // green
    int col = 0;
    while (1) {
        process_put(self, 1, col, 'A', color, 0);
        col = (col + 1) % self->area.w;
        delay();
        schedule();
    }
}

void taskB(void) {
    struct pcb *self = &procs[current];
    int color = 4;  // blue
    int row = 0;
    while (1) {
        process_put(self, row, 5, 'B', color, 0);
        row = (row + 1) % self->area.h;
        delay();
        schedule();
    }
}

/* ------------------------------------------------------------
 * Kernel entry
 * ------------------------------------------------------------ */
int main(void) {
    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 0);
    create_process(taskA, 0, 0, 40, 12);      // upper-left “window”
    create_process(taskB, 40, 12, 40, 12);    // lower-right “window”

    while (1) schedule();
}
