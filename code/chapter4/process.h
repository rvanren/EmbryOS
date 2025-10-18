#ifndef PROCESS_H
#define PROCESS_H

typedef void (*entry_t)(void);

struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

struct pcb {
    void *sp;           // saved stack pointer
    struct rect area;   // allowed screen region
    int priority;       // priority level
    int next;           // next process to run at same priority
};

int proc_init(struct rect area);
void proc_put(struct pcb *p, int row, int col, char ch, int fg, int bg);
int proc_create(struct rect area);

#endif // PROCESS_H
