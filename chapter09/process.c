#include "embryos.h"

struct pcb *proc_create(struct hart *hart, int executable, struct rect area,
                    void *args, int size) {
    struct pcb *pcb = frame_alloc();
    memset(pcb, 0, sizeof(*pcb));
    pcb->hart = hart;
    pcb->executable = executable;
    pcb->area = area;
    pcb->args = args;
    pcb->size = size;
    return pcb;
}

void proc_put(struct pcb *pcb, int col, int row, cell_t cell) {
    if (col < 0 || col >= pcb->area.wd) die("proc_put: bad_col");
    if (row < 0 || row >= pcb->area.ht) die("proc_put: bad row");
    screen_put(pcb->area.col + col, pcb->area.row + row, cell);
}

void proc_enqueue(struct pcb **q, struct pcb *pcb) {
    if (*q == 0) pcb->next = pcb;   // head == tail
    else {
        pcb->next = (*q)->next; // new tail points to head
        (*q)->next = pcb;       // old tail points to new tail
    }
    *q = pcb;   // queue points to tail, tail points to head
}

struct pcb *proc_dequeue(struct pcb **q) {
    struct pcb *pcb = (*q)->next;       // get the head
    if (pcb == pcb->next) *q = 0;       // queue now empty
    else (*q)->next = pcb->next;        // tail points to new head
    return pcb;
}

void proc_release(struct pcb *pcb) {
    if (pcb->base != 0) {
        vm_release(pcb->base);
        frame_release(pcb->base);
    }
    frame_release(pcb);
}
