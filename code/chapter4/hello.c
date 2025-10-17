// chapter4/hello.c
#include "process.h"

// chapter4/timer.c provides these:
void timer_init(void);

static void delay_tiny(void) { for (volatile int i = 0; i < 50000; i++); }

/* Two simple tasks that *do not* call proc_yield().
 * The timer interrupt will preempt them periodically.
 */
void taskA(void) {
    int col = 0;
    int color = 2; // green
    for (;;) {
        proc_put_local(0, col, 'A', color, 0);  // local coords inside its rect
        col = (col + 1) % proc_my_area()->w;
        delay_tiny();    // simulate work
    }
}

void taskB(void) {
    int row = 0;
    int color = 4; // blue
    for (;;) {
        proc_put_local(row, 2, 'B', color, 0);
        row = (row + 1) % proc_my_area()->h;
        delay_tiny();
    }
}

int main(void) {
    // Clear entire screen (black background).
    screen_clear(0, 0, SCREEN_COLS, SCREEN_ROWS, 0);

    proc_init();

    // Four preempted tasks in four regions.
    proc_create(taskA, (struct rect){ 0,   0,  40, 12 });  // upper-left
    proc_create(taskB, (struct rect){ 40,  0,  40, 12 });  // upper-right
    proc_create(taskA, (struct rect){ 0,  12,  40, 12 });  // lower-left
    proc_create(taskB, (struct rect){ 40, 12,  40, 12 });  // lower-right

    // Enable timer + interrupts; from now on, time slices drive preemption.
    timer_init();

    // Idle loop: the timer ISR will do proc_yield() for us.
    for (;;) { __asm__ volatile ("wfi"); }  // wait-for-interrupt
}
