typedef unsigned int uint32_t

struct uart {
    uint32_t txdata;
}

// Memory mapped IO:
#define UART ((struct uart *) 0x10010000)
#define TXFULL (1 << 31)

void putchar(char c) {
    while (UART_TXDATA & TXFULL)
        ;
    UART_TXDATA = c;
}

void printf(char *str) {
    while (*str != 0) {
        putchar(*str++);
    }
}

int main() {
    printf("Hello World\n");
    return 0;
}
