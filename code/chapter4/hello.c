#include "frame.h"
#include "stdio.h"
#include "process.h"
#include "clint.h"
#include "ctx.h"
#include "mtime.h"

#define QUANTUM          100000         // 100 milliseconds
#define N_PRIORITIES     3

int run_queue[N_PRIORITIES];
int proc_current;       // current priority

// The current process yields to the next process to run.
static void yield() {
    // Figure out my process id
    struct pcb *former = FRAME(struct pcb, run_queue[proc_current]);
    int pid = former->next;
    struct pcb *current = FRAME(struct pcb, pid);

    // If the priority of the current process is 0, move the current process
    // to priority 1.  Otherwise keep same priority.
    if (proc_current == 0) {
        // Remove from current priority
        if (former == current) {
            run_queue[0] = -1;
        }
        else {
            former->next = current->next;
        }

        // Insert into next priority
        former = FRAME(struct pcb, run_queue[1]);
        current->next = former->next;
        former->next = pid;
        run_queue[1] = pid;
    }
    else {
        run_queue[proc_current] = pid;
    }

    // Find the next priority to run
    int p = 0;
    while (p < N_PRIORITIES)
        if (run_queue[p] >= 0) break;

    // Switch contexts
    former = FRAME(struct pcb, run_queue[p]);
    struct pcb *next = FRAME(struct pcb, former->next);
    ctx_switch(&current->sp, next->sp);
}

void timer_handler() {
    yield();
    mtime_reset(QUANTUM); // add another quantum
}

static void delay(void) {
    interrupts_enable();
    for (volatile int i = 0; i < 100000; i++)
        yield();
    interrupts_disable();
}

int current_pid() {
    struct pcb *former = FRAME(struct pcb, run_queue[proc_current]);
    return former->next;
}

void taskA(void) {
    struct pcb *self = FRAME(struct pcb, current_pid());
    for (int cnt = 0;; cnt++) {
        proc_put(self, 0, 0, 'A', 2, 0);
        printf(" %x %d", self, cnt);
        delay();
    }
}

static void make_runnable(int pid, int priority) {
    struct pcb *next = FRAME(struct pcb, pid);
    if (run_queue[priority] < 0) {
        run_queue[priority] = pid;
        next->next = pid;
    }
    else {
        struct pcb *former = FRAME(struct pcb, run_queue[priority]);
        next->next = former->next;
        former->next = pid;
    }
}

void run(int pid, entry_t fn) {
    struct pcb *former = FRAME(struct pcb, run_queue[proc_current]);
    struct pcb *current = FRAME(struct pcb, former->next);
    make_runnable(pid, 0);
    ctx_start(&current->sp, &frames[pid].bytes[PAGE_SIZE], fn);
}

void make_unrunnable() {
    struct pcb *former = FRAME(struct pcb, run_queue[proc_current]);
    struct pcb *current = FRAME(struct pcb, former->next);
    if (former == current) {
        run_queue[proc_current] = -1;
    }
    else {
        former->next = current->next;
    }
}

int main(void) {
    frame_init();
    run_queue[0] = proc_init((struct rect){ 0, 0, 80, 24 });
    run_queue[1] = run_queue[2] = -1;
    clint_init();
    clint_set_handler(CLINT_TIMER, timer_handler);

    // mtime_init();
    // mtime_reset(QUANTUM);

    // Allocate processes
    int ul = proc_create((struct rect){ 0,   0,  40, 12 });  // upper-left
    int ur = proc_create((struct rect){ 40,  0,  40, 12 });  // upper-right
    int ll = proc_create((struct rect){ 0,  12,  40, 12 });  // lower-left
    int lr = proc_create((struct rect){ 40, 12,  40, 12 });  // lower-right

    // Run them at priority 0
    run(ul, taskA);
    run(ur, taskA);
    run(ll, taskA);
    run(lr, taskA);

    // Switch priority to level 2
    make_unrunnable();
    make_runnable(current_pid(), 2);
    yield();

    // Run the main loop, which is waiting for interrupts
    interrupts_enable();
    for (;;) {
        __asm__ volatile ("wfi");  // wait-for-interrupt
    }
}
