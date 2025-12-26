#include "embryos.h"
#include "fdt_embryos.h"

#define MAX_HARTS       64
#define QUANTUM         50        // milliseconds

extern uint64_t time_base;                // #ticks per second
extern void startS(uword_t hartid);

struct hart harts[MAX_HARTS];             // one entry for each hart
unsigned int hart_idx;                    // index into harts
extern struct pcb *hart_next;

void timer_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    if (!self->hart->interrupts_work)
        L1(L_BASE, L_INTERRUPTS_WORK, self->hart->id);
    self->hart->interrupts_work = 1;
    sbi_set_timer(mtime_get() + time_base * QUANTUM / 1000);
}

void hart_init(uword_t hartid, struct pcb *self) {
    sched_set_self(self);
    L2(L_BASE, L_HART_INIT, hartid, hart_idx);
    harts[hart_idx].idx = hart_idx;
    harts[hart_idx].id  = hartid;
    hart_idx++;

    // Allocate a process control block for the next hart, if there is any
    hart_next = proc_create(&harts[hart_idx], -1,
                            (struct rect){ 0, 0, 80, 24 }, 0, 0);

    vm_init(self->hart); intr_init();
    if (time_base != 0) sbi_set_timer(0);
}

void hart_start_others(void *fdt) {
    int cpus = fdt_path_offset(fdt, "/cpus");
    if (cpus < 0) die("No /cpus node");

    unsigned int hartidx = 1;
    for (int node = fdt_first_subnode(fdt, cpus); node >= 0;
                         node = fdt_next_subnode(fdt, node)) {
        const char *type = fdt_getprop(fdt, node, "device_type", NULL);
        if (!type || strcmp(type, "cpu") != 0) continue;
        int len;
        const fdt32_t *reg = fdt_getprop(fdt, node, "reg", &len);
        if (reg && len >= 4) {
            uword_t next_hartid = fdt32_to_cpu(*reg);
            kprintf("Next hart: hartid=%d\n", next_hartid);
            if (next_hartid == 0) continue;     // firmware hart or boot hart
            if (next_hartid != harts[0].id && hartidx < MAX_HARTS) {
                hartidx++;
                sbi_hart_start(next_hartid, (uword_t) startS, (uword_t) next_hartid);
            }
        }
    }
}
