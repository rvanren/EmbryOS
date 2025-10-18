#include "frame.h"
#include "stdio.h"
#include "process.h"
#include "clint.h"
#include "ctx.h"
#include "mtime.h"

#define QUANTUM          100000         // 100 milliseconds
#define N_PRIORITIES     3

struct pcb *run_queue[N_PRIORITIES];
int proc_current;       // current priority

void panic(char *s) {
    printf("PANIC %s\n", s);
    for (;;) ;
}

// The current process yields to the next process to run.
static void yield() {
    // If the priority of the current process is 0, move to priority 1.
    if (proc_current == 0) {
        struct pcb *me = proc_dequeue(&run_queue[0]);
        proc_enqueue(&run_queue[1], me);
        proc_current = 1;
    }

    // Move to next process at same priority level
    struct pcb *pcb = run_queue[proc_current] = run_queue[proc_current]->next;

    // Find the highest priority to run
    proc_current = 0;
    while (proc_current < N_PRIORITIES) {
        if (run_queue[proc_current] >= 0) break;
        proc_current++;
    }

    // Switch unless the same
    struct pcb *next = run_queue[proc_current]->next;
    if (next == pcb) return;
    ctx_switch(&pcb->sp, next->sp);
}

void timer_handler() {
    yield();
    mtime_reset(QUANTUM); // add another quantum
}

static void delay(void) {
    interrupts_enable();
    for (volatile int i = 0; i < 100000; i++)
        ;
    interrupts_disable();
}

void taskA(void) {
    struct pcb *self = run_queue[proc_current]->next;
    for (int cnt = 0;; cnt++) {
        proc_put(self, 0, 0, 'A', 2, 0);
        printf("TA %x %d", self, cnt);
        delay();
    }
}

void run(entry_t fn, struct rect area) {
    struct pcb *current = run_queue[proc_current]->next;
    struct pcb *pcb = proc_create(area);
    proc_enqueue(&run_queue[0], pcb);
    proc_current = 0;
    ctx_start(&current->sp, (struct page *) current + 1, fn);
}

int main(void) {
    // Initialize
    frame_init();
    struct pcb *pcb = proc_init((struct rect){ 0, 0, 80, 24 });
    proc_enqueue(&run_queue[0], pcb);
    clint_init();
    clint_set_handler(CLINT_TIMER, timer_handler);
    mtime_init();
    mtime_reset(QUANTUM);

    // Run processes
    run(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    run(taskA, (struct rect){ 40,  0,  40, 12 });  // upper-right
    run(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    run(taskA, (struct rect){ 40, 12,  40, 12 });  // lower-right

    // Switch priority to level 2
    proc_dequeue(&run_queue[proc_current]);
    proc_enqueue(&run_queue[2], pcb);
    proc_current = 2;
    yield();

    // Run the main loop, which is waiting for interrupts at lowest priority
    interrupts_enable();
    for (;;) {
        __asm__ volatile ("wfi");  // wait-for-interrupt
    }
}
