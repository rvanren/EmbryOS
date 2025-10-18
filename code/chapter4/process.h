#ifndef PROCESS_H
#define PROCESS_H

typedef void (*entry_t)(void);

struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

struct pcb {
    struct pcb *next;   // queue management
    int priority;       // priority level
    struct rect area;   // allowed screen region
    void *sp;           // saved stack pointer
};

struct pcb *proc_init(struct rect area);
struct pcb *proc_create(struct rect area);
void proc_put(struct pcb *pcb, int row, int col, char ch, int fg, int bg);
void proc_enqueue(struct pcb **q, struct pcb *pcb);
struct pcb *proc_dequeue(struct pcb **q);

#endif // PROCESS_H
