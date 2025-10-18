#ifndef KBD_H
#define KBD_H

void kbd_init(void);
int  kbd_get(void);        // blocking read
void kbd_isr(void);        // called from interrupt.c

#endif
