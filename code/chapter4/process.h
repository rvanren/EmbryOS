typedef void (*entry_t)(void);

struct rect {
    int x, y;   // top-left corner on global screen
    int w, h;   // width and height
};

struct pcb {
    void *sp;           // saved stack pointer
    struct rect area;   // allowed screen region
};

void proc_init();
struct pcb *proc_self();
void proc_put(struct pcb *p, int row, int col, char ch, int fg, int bg);
void proc_create(entry_t fn, int x, int y, int w, int h);
void proc_yield(void);
