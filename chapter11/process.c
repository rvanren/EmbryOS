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

void proc_release(struct pcb *pcb) {
    if (pcb->base != 0) {
        vm_release(pcb->base);
        frame_release(pcb->base);
    }
    frame_release(pcb);
}
