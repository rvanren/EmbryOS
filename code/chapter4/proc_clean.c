#include <stdint.h>
#include "frame.h"
#include "process.h"
#include "screen.h"
#include "sched.h"
#include "ctx.h"
#include "interrupt.h"
#include "uart.h"

static struct pcb *zombies = 0;   // list of PCBs pending free

void proc_reap_zombies(void) {
    while (zombies != 0) {
        struct pcb *pcb = zombies;
        zombies = zombies->next;
        if (pcb->base != 0) frame_release(pcb->base);
        if (pcb->stack != 0) frame_release(pcb->stack);
        frame_release(pcb);
    }
}
