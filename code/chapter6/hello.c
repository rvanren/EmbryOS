#include "sched.h"
#include "kprintf.h"
#include "syscall.h"
#include "screen.h"
#include "interrupt.h"
#include "frame.h"
#include "mtime.h"
#include "sbi.h"

#define QUANTUM          50000        // 50 milliseconds

void timer_handler(struct trap_frame *tf) {
    sched_yield();
    uint64_t now = mtime_get();
    sbi_set_timer(now + QUANTUM);
}

void exception_handler(struct trap_frame *tf) {
    struct pcb *self = sched_self();
    proc_put(self, 0, 0, CELL('>', ANSI_BLACK, ANSI_RED));
    kprintf("trap: cause=%d sepc=%x stval=%x<",
                        tf->scause & 0xFFF, tf->sepc, tf->stval);
    sched_exit();
}

#define UART0_BASE  0x10000000UL
#define UART_RBR    0x00  // receive buffer
#define UART_LSR    0x05  // line status
#define LSR_DR      0x01  // data ready

static inline int uart_read_ready(void) {
    return (*(volatile uint8_t *)(UART0_BASE + UART_LSR)) & LSR_DR;
}

static inline char uart_getc(void) {
    return *(volatile uint8_t *)(UART0_BASE + UART_RBR);
}

void external_handler(struct trap_frame *tf) {
    while (uart_read_ready()) {
        char c = uart_getc();
        io_putchar(c);
    }
}

int main(void) {
    frame_init(); intr_init();
    intr_set_handler(INTR_EXCEPTION, exception_handler);
    plic_init();
    sched_init(proc_init((struct rect){ 0, 0, 80, 24 }));
    intr_set_handler(INTR_EXTERNAL, external_handler);
    intr_set_handler(INTR_TIMER, timer_handler);
    uint64_t now = mtime_get();
    sbi_set_timer(now + QUANTUM);

    screen_fill(0, 0, SCREEN_COLS, SCREEN_ROWS,
                        CELL(' ', ANSI_WHITE, ANSI_BLACK));

    sched_run(2, (struct rect){ 0, 0, 39, 11 }, 0, 0);  // run init process
    sched_idle();
}
