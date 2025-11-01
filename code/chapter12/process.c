#include <stdint.h>
#include "string.h"
#include "frame.h"
#include "process.h"
#include "screen.h"
#include "vm.h"

struct pcb *proc_init(struct rect area){
    struct pcb *pcb = frame_alloc();
    memset(pcb, 0, sizeof(*pcb));
    pcb->executable = -1;
    pcb->area = area;
    return pcb;
}

struct pcb *proc_create(int executable, struct rect area, void *args, int size) {
    struct pcb *pcb = frame_alloc();
    memset(pcb, 0, sizeof(*pcb));
    pcb->executable = executable;
    pcb->area = area;
    pcb->args = args;
    pcb->size = size;
    return pcb;
}

void proc_put(struct pcb *pcb, int row, int col, cell_t cell) {
    if (row < 0 || row >= pcb->area.h) return;
    if (col < 0 || col >= pcb->area.w) return;
    screen_put(pcb->area.y + row, pcb->area.x + col, cell);
}

void proc_enqueue(struct pcb **q, struct pcb *pcb) {
    if (*q == 0) { *q = pcb; pcb->next = pcb; }
    else { pcb->next = (*q)->next; (*q)->next = pcb; }
}

struct pcb *proc_dequeue(struct pcb **q) {
    struct pcb *pcb = (*q)->next;
    if (pcb == pcb->next) *q = 0;
    else (*q)->next = pcb->next;
    pcb->next = 0;
    return pcb;
}

void proc_release(struct pcb *pcb) {
    if (pcb->base != 0) vm_release(pcb->base);
    frame_release(pcb);
}
