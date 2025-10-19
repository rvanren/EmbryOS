// user.c — simple user program for EmbryOS

extern void putchar(char c);
extern void printf(const char *s);

void main(void) {
    printf("Hello from user mode!\n");
    for (;;)
        ;
}
