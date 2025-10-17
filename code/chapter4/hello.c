#include "stdio.h"
#include "process.h"

#define EXCP_ECALL_U 8
#define EXCP_ECALL_M 11

#define MTIME 0x0200bff8
#define MTIMECMP 0x02004008

#define QUANTUM 1000000

long long mtime_get() {
    // read the current time in microseconds
    return *((long long *)MTIME);
}

void mtime_reset() {
    // signal the CPU after 1 second after the current time
    *((long long *)MTIMECMP) = mtime_get() + QUANTUM;
}

// defined in `hello.s`: is the generic trap handler that saves the registers of
// the user program that was executing. since traps (interrupts/exceptions) are
// abnormal pieces of control flow, the OS does not want to overwrite the
// registers that the user program was using. The OS is also just a program, and
// needs to use these registers!
void _trap_handler();

void handle_syscall() {
    printf("SYSCALL\n");
}

void software_trap_handler() {
    int mcause;
    asm("csrr %0, mcause":"=r"(mcause));

    if (is_interrupt(mcause)) {
        printf("Is Interrupt\n");
        mtime_reset(); // give program another quantum
    } else {
        switch (mcause) {
        case EXCP_ECALL_U: case EXCP_ECALL_M:
            handle_syscall();
            break;
        default:
            printf("Bad Exception\n");
            while(1);
        }
    }
}

// chapter4/timer.c provides these:
void timer_init(void);

/* crude busy-wait delay */
static void delay(void) {
    for (volatile int i = 0; i < 10000000; i++)
        ;
}

void taskA(void) {
    struct pcb *self = proc_self();
    int col = 0;
    while (1) {
        proc_put(self, 1, col, 'A', 2, 0);
        col = (col + 1) % self->area.w;
        delay();
        proc_yield();
    }
}

void taskB(void) {
    struct pcb *self = proc_self();
    int row = 0;
    while (1) {
        proc_put(self, row, 5, 'B', 4, 0);
        row = (row + 1) % self->area.h;
        delay();
        proc_yield();
    }
}

int main(void) {
    proc_init();

    asm("csrw mtvec, %0"::"r"(_trap_handler));
    mtime_reset();
    asm("csrs mstatus, %0"::"r"(1 << 3)); // set MIE=1, enable interrupts to be taken
    asm("csrs mie, %0"::"r"(1 << 7)); // set MTIE=1, unmask timer interrupts

    proc_create(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    proc_create(taskB, (struct rect){ 40,  0,  40, 12 });  // upper-right
    proc_create(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    proc_create(taskB, (struct rect){ 40, 12,  40, 12 });  // lower-right

    timer_init();

    for (;;) { __asm__ volatile ("wfi"); }  // wait-for-interrupt
}
