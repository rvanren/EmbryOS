#include <stdint.h>
#include "frame.h"
#include "process.h"
#include "screen.h"

void proc_put(struct pcb *p, int row, int col, char ch, int fg, int bg) {
    if (row < 0 || row >= p->area.h) return;
    if (col < 0 || col >= p->area.w) return;
    screen_move(p->area.y + row, p->area.x + col);
    screen_put(ch, fg, bg);
}

int proc_init(struct rect area){
    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 0);
    int f = frame_alloc();
    struct pcb *pcb = FRAME(struct pcb, f);
    pcb->priority = 0;
    pcb->next = f;
    return f;
}

int proc_create(struct rect area) {
    int f = frame_alloc();
    struct pcb *pcb = FRAME(struct pcb, f);
    pcb->priority = 0;
    pcb->area = area;
    return f;
}
