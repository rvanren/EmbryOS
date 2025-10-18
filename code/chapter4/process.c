#include <stdint.h>
#include "frame.h"
#include "process.h"
#include "screen.h"

struct pcb *proc_init(struct rect area){
    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 0);
    int f = frame_alloc();
    struct pcb *pcb = FRAME(struct pcb, f);
    pcb->priority = 0;
    pcb->area = area;
    pcb->next = 0;
    return pcb;
}

struct pcb *proc_create(struct rect area) {
    int f = frame_alloc();
    struct pcb *pcb = FRAME(struct pcb, f);
    pcb->priority = 0;
    pcb->area = area;
    pcb->next = 0;
    return pcb;
}

void proc_put(struct pcb *pcb, int row, int col, char ch, int fg, int bg) {
    if (row < 0 || row >= pcb->area.h) return;
    if (col < 0 || col >= pcb->area.w) return;
    screen_move(pcb->area.y + row, pcb->area.x + col);
    screen_put(ch, fg, bg);
}

void proc_enqueue(struct pcb **q, struct pcb *pcb) {
    if (*q == 0) { *q = pcb; pcb->next = pcb; }
    else { pcb->next = (*q)->next; (*q)->next = pcb; }
}

struct pcb *proc_dequeue(struct pcb **q) {
    struct pcb = (*q)->next;
    if (pcb == pcb->next) *q = 0;
    else (*q)->next = pcb->next;
    pcb->next = 0;
    return pcb;
}
