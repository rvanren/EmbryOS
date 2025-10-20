#include <stdint.h>
#include "frame.h"
#include "process.h"
#include "screen.h"
#include "sched.h"
#include "ctx.h"
#include "interrupt.h"

static struct pcb *zombies = 0;   // list of PCBs pending free

void proc_reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        frame_release(pcb);
    }
}

void proc_exit(void) {
    struct pcb *pcb = proc_dequeue(&run_queue[proc_current]);
    pcb->next = zombies;
    zombies = pcb;
    sched_block(pcb);
}
