#include <stdint.h>
#include "frame.h"
#include "process.h"
#include "screen.h"
#include "sched.h"      // run_queue, proc_current
#include "ctx.h"        // ctx_switch
#include "interrupt.h"  // intr_disable/intr_enable if available

static struct pcb *zombies = 0;   // LIFO list of PCBs pending free

void proc_reap_zombies(void) {
    // Reap on the *current* stack (safe, because none of these PCBs is current)
    while (zombies) {
        struct pcb *p = zombies;
        zombies = zombies->next;

        // PCB + kernel stack live in the same frame allocated in proc_create()
        frame_release(p);
    }
}

// Never returns; switches to the next runnable thread.
void proc_exit(void) {
    intr_disable();

    // Remove current from its run queue.
    int q = proc_current;
    struct pcb *dead = proc_dequeue(&run_queue[q]);

    // Push onto zombie list for later freeing (on a different stack).
    dead->next = zombies;
    zombies = dead;

    // Pick the next runnable priority.
    proc_current = 0;
    while (proc_current < N_PRIORITIES && run_queue[proc_current] == 0)
        proc_current++;

    // If nothing is runnable, just wait for interrupts forever.
    if (proc_current >= N_PRIORITIES) {
        for (;;) { __asm__ volatile ("wfi"); }
    }

    // Switch away from the soon-to-be-freed stack. We never return here.
    struct pcb *next = run_queue[proc_current]->next;
    ctx_switch(&dead->sp, next->sp);
}
