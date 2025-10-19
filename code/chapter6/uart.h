#ifndef UART_H
#define UART_H

void putchar(char c);
void uart_init(void);
int  uart_get(void);
void uart_isr();

#endif // UART_H
